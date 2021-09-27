#pragma once
using namespace Windows::UI;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Windows::UI::Xaml::Controls;
using namespace Microsoft::Graphics::Canvas::UI::Xaml;

class Engine
{
public:
	struct BoundingBox
	{
		float x;
		float y;
		float width;
		float height;
	};

	struct EngineTexture 
	{
		CanvasBitmap^ bitmap;
	};

	Engine();
	Engine(CanvasAnimatedControl^ canvas, Grid^ grid);

	void SetDrawingSession(CanvasDrawingSession^ canvasDrawingSession);

	void Clear(Color color);
	void Rect(float x, float y, float width, float height, Color color);
	void Text(float x, float y, Platform::String^ text, float fontSize, Color color);
	void Ellipse(float x, float y, float radiusX, float radiusY, Color color);
	void DrawBoundingBox(BoundingBox bBox);
	void Line(float x1, float y1, float x2, float y2, Color color);

	Engine::EngineTexture Engine::GenerateTexture(Platform::String^ path);

	/// <param name="opacity">is between 0 and 1</param>
	void Engine::TexturedRect(Engine::EngineTexture texture, float x, float y, float width, float height, float opacity = 1);

	BoundingBox CreateBoundingBox(float x, float y, float width, float height);
	bool IntersectAABB(BoundingBox b1, BoundingBox b2);
};