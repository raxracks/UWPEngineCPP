#include "Engine.h";
#include <chrono>
#include <vector>

using namespace Windows::Gaming::Input;
using namespace std::chrono;
using namespace Windows::UI::Popups;
using namespace Windows::Foundation;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Microsoft::Graphics::Canvas;
using namespace Windows::UI::Xaml::Controls;
using namespace Microsoft::Graphics::Canvas::UI::Xaml;

CanvasAnimatedControl^ cnvs;
Grid^ grd;
CanvasDrawingSession^ cds;
milliseconds time1;
milliseconds time2;

Gamepad^ gamepad;

bool gamepadConnected = false;

bool DrawingSessionClosed()
{
	if (cds == nullptr) return true;

	try
	{
		cds->DrawLine(0, 0, 0, 0, Colors::White);

		return false;
	}
	catch (const char* e) {
		return true;
	}
}

Engine::Engine()
{

}

Engine::Engine(CanvasAnimatedControl^ canvas, Grid^ grid)
{
	cnvs = canvas;
	grd = grid;

	Gamepad::GamepadAdded += ref new Windows::Foundation::EventHandler<Windows::Gaming::Input::Gamepad^>(&OnGamepadAdded);
	Gamepad::GamepadRemoved += ref new Windows::Foundation::EventHandler<Windows::Gaming::Input::Gamepad^>(&OnGamepadRemoved);
}

void Engine::SetDrawingSession(CanvasDrawingSession^ canvasDrawingSession)
{
	cds = canvasDrawingSession;
}

void Engine::Clear(Color color) {
	if (DrawingSessionClosed()) return;

	cds->Clear(color);
}

void Engine::Rect(float x, float y, float width, float height, Color color)
{
	if (DrawingSessionClosed()) return;

	cds->FillRectangle(x, y, width, height, color);
}

Color Engine::ARGB(float a, float r, float g, float b) {
	Color color;

	color.A = a;
	color.R = r;
	color.G = g;
	color.B = b;

	return color;
}

void Engine::UnfilledRect(float x, float y, float width, float height, Color color, float thickness, float border_radius)
{
	if (DrawingSessionClosed()) return;

	cds->DrawRoundedRectangle(x, y, width, height, border_radius, border_radius, color, thickness);
}

float Engine::CalculateDeltaTime() {
	time1 = time2;

	time2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	duration<float> difference = time2 - time1;
	float dt = difference.count();

	if (dt > 10) dt = 0;

	return dt;
}

void Engine::DeclareEndUpdate() {
	time1 = time2;
}

void Engine::Text(float x, float y, Platform::String^ text, float fontSize, Color color, CanvasHorizontalAlignment textAlignment, float width)
{
	if (DrawingSessionClosed()) return;

	CanvasTextFormat^ format = ref new CanvasTextFormat();

	format->FontSize = fontSize;
	format->WordWrapping = CanvasWordWrapping::NoWrap;
	format->HorizontalAlignment = textAlignment;

	Windows::Foundation::Rect rect;
	rect.X = x;
	rect.Y = y;
	rect.Width = width;

	if (width < -1)
	{
		switch (textAlignment)
		{
		case CanvasHorizontalAlignment::Center:
			rect.X = x + (width / 2) - (GetStringSizePX(text, fontSize, textAlignment).Width / 2);
			break;

		case CanvasHorizontalAlignment::Right:
			rect.X = x + width - GetStringSizePX(text, fontSize, textAlignment).Width;
			break;

		default:
			break;
		}
	}

	cds->DrawText(text, x, y, color, format);
}

Windows::Foundation::Rect Engine::GetStringSizePX(Platform::String^ text, float fontSize, CanvasHorizontalAlignment textAlignment)
{
	if (DrawingSessionClosed()) return Windows::Foundation::Rect();

	CanvasTextFormat^ format = ref new CanvasTextFormat;
	format->FontSize = fontSize;
	format->WordWrapping = CanvasWordWrapping::NoWrap;
	format->HorizontalAlignment = textAlignment;

	CanvasTextLayout^ textLayout = ref new CanvasTextLayout(cds, text, format, 0.0f, 0.0f);
	textLayout->WordWrapping = CanvasWordWrapping::NoWrap;

	Windows::Foundation::Rect rect = textLayout->LayoutBounds;

	return rect;
}

void Engine::Ellipse(float x, float y, float radiusX, float radiusY, Color color)
{
	if (DrawingSessionClosed()) return;

	cds->DrawEllipse(x, y, radiusX, radiusY, color);
}

void Engine::DrawBoundingBox(BoundingBox bBox) {
	if (DrawingSessionClosed()) return;

	Rect(bBox.x, bBox.y, bBox.width, bBox.height, ARGB(100, 191, 242, 133));
}

void Engine::Line(float x1, float y1, float x2, float y2, Color color) {
	if (DrawingSessionClosed()) return;

	cds->DrawLine(x1, y1, x2, y2, color);
}

Engine::EngineTexture Engine::GenerateTexture(Platform::String^ path) {
	try {
		EngineTexture texture;

		auto load = CanvasBitmap::LoadAsync(cnvs, path);

		while (load->Status != Windows::Foundation::AsyncStatus::Completed) {
			if (load->Status == Windows::Foundation::AsyncStatus::Error || load->Status == Windows::Foundation::AsyncStatus::Canceled) {
				MessageDialog("Image failed to load.\nPath: " + path).ShowAsync();

				CanvasBitmap^ bitmap = GenerateTexture("Assets/StoreLogo.png").bitmap;

				texture.bitmap = bitmap;
			}
		}

		texture.bitmap = load->GetResults();

		return texture;
	}
	catch (const char e) {
		MessageDialog(e.ToString()).ShowAsync();
	}
}

void Engine::TexturedRect(Engine::EngineTexture texture, float x, float y, float width, float height, float opacity) {

	Windows::Foundation::Rect rect;
	rect.X = x;
	rect.Y = y;
	rect.Width = width;
	rect.Height = height;

	cds->DrawImage(texture.bitmap, rect, texture.bitmap->GetBounds(cnvs), opacity);
}

Engine::BoundingBox Engine::CreateBoundingBox(float x, float y, float width, float height)
{
	BoundingBox bBox = BoundingBox();
	bBox.x = x;
	bBox.y = y;
	bBox.width = width;
	bBox.height = height;

	return bBox;
}

bool Engine::IntersectAABB(BoundingBox b1, BoundingBox b2)
{
	return b1.x < b2.x + b2.width && b1.x + b1.width > b2.x && b1.y < b2.y + b2.height && b1.y + b1.height > b2.y;
}

Engine::GamepadData Engine::GetGamepad() {
	Engine::GamepadData data;

	if (gamepadConnected) {
		GamepadReading reading = gamepad->GetCurrentReading();

		data.LT = reading.LeftTrigger;
		data.RT = reading.RightTrigger;
		data.LSX = reading.LeftThumbstickX;
		data.LSY = reading.LeftThumbstickY;
		data.RSX = reading.RightThumbstickX;
		data.RSY = reading.RightThumbstickY;
		data.A = (reading.Buttons & GamepadButtons::A) == GamepadButtons::A;
		data.B = (reading.Buttons & GamepadButtons::B) == GamepadButtons::B;
		data.X = (reading.Buttons & GamepadButtons::X) == GamepadButtons::X;
		data.Y = (reading.Buttons & GamepadButtons::Y) == GamepadButtons::Y;
		data.LB = (reading.Buttons & GamepadButtons::LeftShoulder) == GamepadButtons::LeftShoulder;
		data.RB = (reading.Buttons & GamepadButtons::RightShoulder) == GamepadButtons::RightShoulder;
		data.LS = (reading.Buttons & GamepadButtons::LeftThumbstick) == GamepadButtons::LeftThumbstick;
		data.RS = (reading.Buttons & GamepadButtons::RightThumbstick) == GamepadButtons::RightThumbstick;
		data.DPadLeft = (reading.Buttons & GamepadButtons::DPadLeft) == GamepadButtons::DPadLeft;
		data.DPadRight = (reading.Buttons & GamepadButtons::DPadRight) == GamepadButtons::DPadRight;
		data.DPadUp = (reading.Buttons & GamepadButtons::DPadUp) == GamepadButtons::DPadUp;
		data.DPadDown = (reading.Buttons & GamepadButtons::DPadDown) == GamepadButtons::DPadDown;
		data.Connected = true;
	}
	else {
		data.LT = 0;
		data.RT = 0;
		data.LSX = 0;
		data.LSY = 0;
		data.RSX = 0;
		data.RSY = 0;
		data.A = false;
		data.B = false;
		data.X = false;
		data.Y = false;
		data.LB = false;
		data.RB = false;
		data.LS = false;
		data.RS = false;
		data.DPadLeft = false;
		data.DPadRight = false;
		data.DPadUp = false;
		data.DPadDown = false;
		data.Connected = false;
	}

	return data;
}

void Engine::OnGamepadAdded(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ args)
{
	gamepad = args;
	gamepadConnected = true;
}

void Engine::OnGamepadRemoved(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ args) {
	gamepad = nullptr;
	gamepadConnected = false;
}