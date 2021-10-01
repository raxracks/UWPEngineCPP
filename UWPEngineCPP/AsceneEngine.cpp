#include <AsceneEngine.h>
#include <chrono>
#include <vector>
#include <string>
#include <Windows.h>

using namespace Microsoft::Graphics::Canvas::Geometry;

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
CanvasDrawingSession^ frontcds;

milliseconds time1;
milliseconds time2;

Gamepad^ gamepad;

std::vector<AsceneEngine::InformerData> dataLog;

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

AsceneEngine::AsceneEngine()
{

}

AsceneEngine::AsceneEngine(CanvasAnimatedControl^ canvas, Grid^ grid)
{
	cnvs = canvas;
	grd = grid;

	Gamepad::GamepadAdded += ref new Windows::Foundation::EventHandler<Windows::Gaming::Input::Gamepad^>(&OnGamepadAdded);
	Gamepad::GamepadRemoved += ref new Windows::Foundation::EventHandler<Windows::Gaming::Input::Gamepad^>(&OnGamepadRemoved);
}

void AsceneEngine::SetDrawingSession(CanvasDrawingSession^ canvasDrawingSession)
{
	cds = canvasDrawingSession;
}

void AsceneEngine::Clear(Color color) {
	if (DrawingSessionClosed()) return;

	int i = InformObserver("Clear", AsceneEngine::InformerState::ProcessStart);
	cds->Clear(color);
	InformObserver("Clear", AsceneEngine::InformerState::ProcessFinish, i);
}

void AsceneEngine::Rect(float x, float y, float width, float height, Color color, float radius)
{
	if (DrawingSessionClosed()) return;

	int i = InformObserver("Rect", AsceneEngine::InformerState::ProcessStart);
	cds->FillRoundedRectangle(x, y, width, height, radius, radius, color);
	InformObserver("Rect", AsceneEngine::InformerState::ProcessFinish, i);
}

void AsceneEngine::Triangles(std::vector<float2> vertices, Color color) {
	CanvasPathBuilder^ pathBuilder = ref new CanvasPathBuilder(cnvs);
	std::vector<float2> trianglePoints;

	for (int i = 0; i < sizeof(vertices); i += 3) {
		trianglePoints[i] = vertices[i];
		trianglePoints[i + 1] = vertices[i + 1];
		trianglePoints[i + 2] = vertices[i + 2];

		Platform::Array<float2>^ data = ref new Platform::Array<float2>(trianglePoints.data(),
			trianglePoints.size());

		CanvasGeometry^ triangleGeometry = CanvasGeometry::CreatePolygon((ICanvasResourceCreator^)cnvs->Device, data);

		pathBuilder->AddGeometry(triangleGeometry);
	}

	cds->FillGeometry(CanvasGeometry::CreatePath(pathBuilder), color);
}

void AsceneEngine::FlatBottomTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Color color) {
	float invslope1 = (x1 - x0) / (y1 - y0);
	float invslope2 = (x2 - x0) / (y2 - y0);

	float curx1 = x0;
	float curx2 = x0;

	for (int scanlineY = y0; scanlineY <= y1; scanlineY++)
	{
		Line((int)curx1, scanlineY, (int)curx2, scanlineY, color);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}

void AsceneEngine::FlatTopTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Color color) {
	float invslope1 = (x2 - x0) / (y2 - y0);
	float invslope2 = (x2 - x1) / (y2 - y1);

	float curx1 = x2;
	float curx2 = x2;

	for (int scanlineY = y2; scanlineY > y1; scanlineY--)
	{
		Line((int)curx1, scanlineY, (int)curx2, scanlineY, color);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}

Color AsceneEngine::ARGB(float a, float r, float g, float b) {
	Color color;

	color.A = a;
	color.R = r;
	color.G = g;
	color.B = b;

	return color;
}

void AsceneEngine::UnfilledRect(float x, float y, float width, float height, Color color, float thickness, float border_radius)
{
	if (DrawingSessionClosed()) return;

	int i = InformObserver("UnfilledRect", AsceneEngine::InformerState::ProcessStart);
	cds->DrawRoundedRectangle(x, y, width, height, border_radius, border_radius, color, thickness);
	InformObserver("UnfilledRect", AsceneEngine::InformerState::ProcessFinish, i);
}

milliseconds getEpochMS() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
};

microseconds getEpochMiS() {
	return duration_cast<microseconds>(system_clock::now().time_since_epoch());
};

float AsceneEngine::CalculateDeltaTime() {
	time1 = time2;

	time2 = getEpochMS();

	duration<float> difference = time2 - time1;
	float dt = difference.count();

	if (dt > 10) dt = 0;

	return dt;
}

void AsceneEngine::Text(float x, float y, Platform::String^ text, float fontSize, Color color, CanvasHorizontalAlignment textAlignment, float width)
{
	if (DrawingSessionClosed()) return;

	int i = InformObserver("Text", AsceneEngine::InformerState::ProcessStart);
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
	InformObserver("Text", AsceneEngine::InformerState::ProcessFinish, i);
}

Windows::Foundation::Rect AsceneEngine::GetStringSizePX(Platform::String^ text, float fontSize, CanvasHorizontalAlignment textAlignment)
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

void AsceneEngine::Ellipse(float x, float y, float radiusX, float radiusY, Color color)
{
	if (DrawingSessionClosed()) return;

	int i = InformObserver("Ellipse", AsceneEngine::InformerState::ProcessStart);
	cds->DrawEllipse(x, y, radiusX, radiusY, color);
	InformObserver("Ellipse", AsceneEngine::InformerState::ProcessFinish, i);
}

void AsceneEngine::DrawBoundingBox(BoundingBox bBox) {
	if (DrawingSessionClosed()) return;

	int i = InformObserver("DrawBoundingBox", AsceneEngine::InformerState::ProcessStart);
	Rect(bBox.x, bBox.y, bBox.width, bBox.height, ARGB(100, 191, 242, 133));
	InformObserver("DrawBoundingBox", AsceneEngine::InformerState::ProcessFinish, i);
}

void AsceneEngine::Line(float x1, float y1, float x2, float y2, Color color) {
	if (DrawingSessionClosed()) return;

	int i = InformObserver("Line", AsceneEngine::InformerState::ProcessStart);
	cds->DrawLine(x1, y1, x2, y2, color);
	InformObserver("DrawBoundingBox", AsceneEngine::InformerState::ProcessFinish, i);
}

AsceneEngine::EngineTexture AsceneEngine::GenerateTexture(Platform::String^ path) {
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

void AsceneEngine::TexturedRect(AsceneEngine::EngineTexture texture, float x, float y, float width, float height, float opacity) {
	if (DrawingSessionClosed()) return;

	int i = InformObserver("TexturedRect", AsceneEngine::InformerState::ProcessStart);
	Windows::Foundation::Rect rect;
	rect.X = x;
	rect.Y = y;
	rect.Width = width;
	rect.Height = height;

	cds->DrawImage(texture.bitmap, rect, texture.bitmap->GetBounds(cnvs), opacity);
	InformObserver("TexturedRect", AsceneEngine::InformerState::ProcessFinish, i);
}

AsceneEngine::BoundingBox AsceneEngine::CreateBoundingBox(float x, float y, float width, float height)
{
	BoundingBox bBox = BoundingBox();
	bBox.x = x;
	bBox.y = y;
	bBox.width = width;
	bBox.height = height;

	return bBox;
}

bool AsceneEngine::IntersectAABB(BoundingBox b1, BoundingBox b2)
{
	return b1.x < b2.x + b2.width && b1.x + b1.width > b2.x && b1.y < b2.y + b2.height && b1.y + b1.height > b2.y;
}

AsceneEngine::GamepadData AsceneEngine::GetGamepad() {
	AsceneEngine::GamepadData data;

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

int AsceneEngine::InformObserver(Platform::String^ process, AsceneEngine::InformerState state, int index) {
	AsceneEngine::InformerData data;
	
	switch (state) {
		case AsceneEngine::InformerState::ProcessStart:
			data.ProcessName = process->ToString();
			data.State = state;
			data.StartTime = getEpochMiS();

			index = (int)dataLog.size();

			dataLog.push_back(data);
			
			break;

		case AsceneEngine::InformerState::ProcessFinish:
			data = dataLog[index];
			data.StopTime = getEpochMiS();
			data.State = state;
			dataLog[index] = data;
			break;
	}

	return index;
}

std::vector<AsceneEngine::InformerData> AsceneEngine::Debug::GetLog() {
	return dataLog;
}

void AsceneEngine::LogMessage(Platform::Object^ parameter)
{
	auto paraString = parameter->ToString();
	auto formattedText = std::wstring(paraString->Data()).append(L"\r\n");
	OutputDebugString(formattedText.c_str());
}

void AsceneEngine::StopEngine() {
	//disabled = true;
}

void AsceneEngine::OnGamepadAdded(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ args)
{
	gamepad = args;
	gamepadConnected = true;
}

void AsceneEngine::OnGamepadRemoved(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ args) {
	gamepad = nullptr;
	gamepadConnected = false;
}