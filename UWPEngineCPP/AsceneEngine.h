#pragma once
#include <chrono>
#include <vector>

using namespace Windows::UI;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Windows::UI::Xaml::Controls;
using namespace Microsoft::Graphics::Canvas::UI::Xaml;
using namespace Windows::Foundation::Numerics;

class AsceneEngine
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

	enum InformerState {
		ProcessStart,
		ProcessFinish
	};

	struct InformerData {
		Platform::String^ ProcessName;
		InformerState State;
		std::chrono::microseconds StartTime;
		std::chrono::microseconds StopTime;
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

	AsceneEngine();
	AsceneEngine(CanvasAnimatedControl^ canvas, Grid^ grid);

	void SetDrawingSession(CanvasDrawingSession^ canvasDrawingSession);
	void Clear(Color color);
	void Rect(float x, float y, float width, float height, Color color, float radius = 0);
	void UnfilledRect(float x, float y, float width, float height, Color color, float thickness = 1, float border_radius = 0);
	void Text(float x, float y, Platform::String^ text, float fontSize, Color color, CanvasHorizontalAlignment textAlignment = CanvasHorizontalAlignment::Left, float width = -1);
	void Ellipse(float x, float y, float radiusX, float radiusY, Color color);
	void DrawBoundingBox(BoundingBox bBox);
	void Line(float x1, float y1, float x2, float y2, Color color);
	void StopEngine();

	/// <param name="opacity">is between 0 and 1</param>
	void TexturedRect(AsceneEngine::EngineTexture texture, float x, float y, float width, float height, float opacity = 1);

	bool IntersectAABB(BoundingBox b1, BoundingBox b2);

	float CalculateDeltaTime();

	Color ARGB(float a, float r, float g, float b);

	AsceneEngine::EngineTexture GenerateTexture(Platform::String^ path);

	AsceneEngine::GamepadData GetGamepad();

	BoundingBox CreateBoundingBox(float x, float y, float width, float height);

	Windows::Foundation::Rect GetStringSizePX(Platform::String^ text, float fontSize, CanvasHorizontalAlignment textAlignment = CanvasHorizontalAlignment::Left);

	void Triangles(std::vector<float2> vertices, Color color);

	void FlatBottomTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Color color);
	void FlatTopTriangle(float x0, float y0, float x1, float y1, float x2, float y2, Color color);

	void LogMessage(Platform::Object^ parameter);

	static class Debug {
	public:
		static std::vector<AsceneEngine::InformerData> GetLog();
	};

private:
	static void OnGamepadAdded(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ args);
	static void OnGamepadRemoved(Platform::Object^ sender, Windows::Gaming::Input::Gamepad^ args);
	
	int InformObserver(Platform::String^ process, AsceneEngine::InformerState state, int index = -1);
};