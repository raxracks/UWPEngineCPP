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

	struct GamepadData
	{
		float LT;
		float RT;
		float LSX;
		float LSY;
		float RSX;
		float RSY;
		bool A;
		bool B;
		bool X;
		bool Y;
		bool LB;
		bool RB;
		bool LS;
		bool RS;
		bool DPadLeft;
		bool DPadRight;
		bool DPadUp;
		bool DPadDown;
		bool Connected;
	};

	Engine();
	Engine(CanvasAnimatedControl^ canvas, Grid^ grid);

	void SetDrawingSession(CanvasDrawingSession^ canvasDrawingSession);

	void Clear(Color color);
	void Rect(float x, float y, float width, float height, Color color);
	void UnfilledRect(float x, float y, float width, float height, Color color, float thickness = 1, float border_radius = 0);
	void Text(float x, float y, Platform::String^ text, float fontSize, Color color, CanvasHorizontalAlignment textAlignment = CanvasHorizontalAlignment::Left, float width = -1);
	Windows::Foundation::Rect GetStringSizePX(Platform::String^ text, float fontSize, CanvasHorizontalAlignment textAlignment = CanvasHorizontalAlignment::Left);
	void Ellipse(float x, float y, float radiusX, float radiusY, Color color);
	void DrawBoundingBox(BoundingBox bBox);
	void Line(float x1, float y1, float x2, float y2, Color color);

	Color ARGB(float a, float r, float g, float b);

	Engine::EngineTexture Engine::GenerateTexture(Platform::String^ path);

	Engine::GamepadData GetGamepad();

	/// <param name="opacity">is between 0 and 1</param>
	void Engine::TexturedRect(Engine::EngineTexture texture, float x, float y, float width, float height, float opacity = 1);

	BoundingBox CreateBoundingBox(float x, float y, float width, float height);
	bool IntersectAABB(BoundingBox b1, BoundingBox b2);

	void Engine::DeclareEndUpdate();
	float Engine::CalculateDeltaTime();

private:
	static void OnGamepadAdded(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ args);
	static void OnGamepadRemoved(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ args);
};