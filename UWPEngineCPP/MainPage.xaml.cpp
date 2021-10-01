//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <timercpp.h>
#include <AsceneEngine.h>
#include <pender_ui.h>

using namespace Windows::Gaming::Input;
using namespace std::chrono;
using namespace Windows::UI::Popups;
using namespace EngineExample;
using namespace Windows::UI;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Input;

AsceneEngine engine;

Pender pender;

int frameCount = 0;
int fps = 0;

float spriteX = 475;
float spriteY = -10;
float objectX = 450;
float objectY = 120;

float mouseX = 0;
float mouseY = 0;

bool mouseDown = false;

float totalLogTime = 0;

/*                  Textures                    */
AsceneEngine::EngineTexture aButtonTexture;
AsceneEngine::EngineTexture bButtonTexture;
AsceneEngine::EngineTexture xButtonTexture;
AsceneEngine::EngineTexture yButtonTexture;
AsceneEngine::EngineTexture spriteTexture;
/*              End of textures                 */

/*              Bounding Boxes                  */
AsceneEngine::BoundingBox spriteBoundingBox;
AsceneEngine::BoundingBox objectBoundingBox;
/*           End of bounding boxes              */

Timer timer;

std::vector<AsceneEngine::InformerData> logCopy;

Size renderingSize;

float deltaTime;

bool debugging = false;

MainPage::MainPage()
{
	InitializeComponent();

	engine = AsceneEngine(canvas, grid);
}

void EngineExample::MainPage::canvas_CreateResources(Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::CanvasCreateResourcesEventArgs^ args)
{
	/*                  Textures                    */
	aButtonTexture = engine.GenerateTexture("Assets/GameAssets/GamepadIcons/A.png");
	bButtonTexture = engine.GenerateTexture("Assets/GameAssets/GamepadIcons/B.png");
	xButtonTexture = engine.GenerateTexture("Assets/GameAssets/GamepadIcons/X.png");
	yButtonTexture = engine.GenerateTexture("Assets/GameAssets/GamepadIcons/Y.png");
	spriteTexture = engine.GenerateTexture("Assets/GameAssets/cookie.png");
	/*              End of textures                 */

	// FPS Calculations
	timer.setInterval([&]() {
		fps = frameCount;
		frameCount = 0;
	}, 1000);

	Pender::UIConfig config;
	config.fontSize = 15;
	config.padding = 8;
	config.titleFontSize = 12;
	config.windowRadius = 5;
	config.closeIconPadding = 20;

	Pender::ColorScheme scheme;
	scheme.windowBgColor = engine.ARGB(255, 21, 22, 23);
	scheme.textColor = Colors::White;
	scheme.titleColor = engine.ARGB(255, 41, 74, 122);

	pender.Init(engine, config, scheme);
}

void EngineExample::MainPage::canvas_Draw(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
	renderingSize = sender->Size;

	// Update drawing session
	engine.SetDrawingSession(args->DrawingSession);

	if (debugging) {
		engine.Clear(Colors::White);

		float x = 0;
		float y = 0;

		for (AsceneEngine::InformerData d : logCopy) {
			if (d.State == AsceneEngine::InformerState::ProcessFinish) {
				long long time = (d.StopTime - d.StartTime).count();
				float width = time + engine.GetStringSizePX(d.ProcessName, 10).Width;

				if (x + width > renderingSize.Width) {
					x = 0;
					y += 41;
				}

				engine.Rect(x, y, width, 40, Colors::Black);
				engine.Text(x, y, d.ProcessName, 10, Colors::White);
				engine.Text(x, y + 20, time.ToString() + "MiS", 10, Colors::White);
				x += width + 1;

				if (x > renderingSize.Width) {
					x = 0;
					y += 41;
				}
			}
		}
	}
	else {

		// Clear rendering area with specified color
		engine.Clear(Colors::CornflowerBlue);

		// Draw text
		engine.Text(10, 10, "Hello World!", 50, Colors::Black);

		// Draw a textured rectangle
		engine.TexturedRect(aButtonTexture, 10, 90, 30, 30);

		// Draw an unfilled rectangle
		engine.UnfilledRect(10, 140, 100, 50, Colors::Black, 5, 10);

		// Draw a filled rectangle
		engine.Rect(120, 140, 100, 50, Colors::Orange);

		// Draw sprite and floor
		engine.TexturedRect(spriteTexture, spriteX, spriteY, 50, 50);
		engine.Rect(objectX, objectY, 200, 20, Colors::Green);

		// Display FPS
		Rect FPSRect = engine.GetStringSizePX("FPS: " + fps, 25, CanvasHorizontalAlignment::Right);
		engine.Text((float)renderingSize.Width - (float)FPSRect.Width, (float)renderingSize.Height - (float)FPSRect.Height, "FPS: " + fps, 25, Colors::Black);

		// Increment frame count for FPS calculations
		frameCount++;
	}

	pender.Begin("window title bruh momento numero dos");
	pender.Text("hi");
	pender.Text("more text bla bla bla bla");
	pender.End();
}


void EngineExample::MainPage::canvas_Update(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedUpdateEventArgs^ args)
{
    // Calculate deltaTime
	deltaTime = engine.CalculateDeltaTime();

	pender.UpdateMouse(mouseX, mouseY, mouseDown);

    // Check if colliding 
    if (engine.IntersectAABB(spriteBoundingBox, objectBoundingBox))
    {
        // Stop sprite from falling through object (newton's third law)
        spriteY -= 100 * deltaTime;
    }

    // Controls
	if (engine.GetGamepad().DPadRight)
	{
		spriteX += 100 * deltaTime;
	}

	if (engine.GetGamepad().DPadLeft)
	{
		spriteX -= 100 * deltaTime;
	}

	if (engine.GetGamepad().DPadUp)
	{
		spriteY -= 200 * deltaTime;
	}

	if (engine.GetGamepad().DPadDown)
	{
		spriteY += 100 * deltaTime;
	}

    // Super simple "gravity" on the sprite
    spriteY += 100 * deltaTime;

    // Draw sprite and create a bounding box for it
    spriteBoundingBox = engine.CreateBoundingBox(spriteX, spriteY, 50, 50);

    // Draw an object to stop the sprite from falling and create a bounding box for it
    objectBoundingBox = engine.CreateBoundingBox(objectX, objectY, 200, 20);
}

/*void EngineExample::MainPage::grid_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	logCopy = AsceneEngine::Debug::GetLog();

	debugging = true;

	//engine.Rect(0, renderingSize.Height - 100, renderingSize.Width, 100, Colors::White);

	//engine.StopEngine();
}*/


void EngineExample::MainPage::canvas_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	PointerPoint^ p = e->GetCurrentPoint(canvas);
	mouseX = p->Position.X;
	mouseY = p->Position.Y;
}


void EngineExample::MainPage::canvas_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	mouseDown = true;
}


void EngineExample::MainPage::canvas_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	mouseDown = false;
}
