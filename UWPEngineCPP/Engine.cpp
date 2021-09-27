#include "Engine.h";
#include <chrono>

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

bool DrawingSessionClosed()
{
    if (cds == nullptr) return true;

    try
    {
        cds->DrawLine(0, 0, 0, 0, Colors::White);

        return false;
    } catch(const char *e) {
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

void Engine::UnfilledRect(float x, float y, float width, float height, Color color)
{
    if (DrawingSessionClosed()) return;

    cds->DrawRectangle(x, y, width, height, color);
}

float Engine::CalculateDeltaTime() {
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

    //CanvasTextFormat format = ref new CanvasTextFormat{ FontSize = fontSize, WordWrapping = CanvasWordWrapping.NoWrap, HorizontalAlignment = textAlignment };

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

    Color color;
    color.A = 100;
    color.R = 191;
    color.G = 242;
    color.B = 133;

    Rect(bBox.x, bBox.y, bBox.width, bBox.height, color);
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
