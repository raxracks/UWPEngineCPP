#include "Engine.h";
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

void Engine::Text(float x, float y, Platform::String^ text, float fontSize, Color color)
{
    if (DrawingSessionClosed()) return;

    cds->DrawText(text, x, y, color);
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
