//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <Engine.h>
#include <timercpp.h>
using namespace Windows::Gaming::Input;

using namespace std::chrono;
using namespace Windows::UI::Popups;
using namespace UWPEngine;
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

Engine engine;

int frameCount = 0;
int fps = 0;

float spriteX = 475;
float spriteY = -10;
float objectX = 450;
float objectY = 120;

/*                  Textures                    */
Engine::EngineTexture aButtonTexture;
Engine::EngineTexture bButtonTexture;
Engine::EngineTexture xButtonTexture;
Engine::EngineTexture yButtonTexture;
Engine::EngineTexture spriteTexture;
/*              End of textures                 */

/*              Bounding Boxes                  */
Engine::BoundingBox spriteBoundingBox;
Engine::BoundingBox objectBoundingBox;
/*           End of bounding boxes              */

float deltaTime;

MainPage::MainPage()
{
	InitializeComponent();

	engine = Engine(canvas, grid);
}

void UWPEngine::MainPage::canvas_CreateResources(Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::CanvasCreateResourcesEventArgs^ args)
{
	/*                  Textures                    */
	aButtonTexture = engine.GenerateTexture("Assets/GameAssets/GamepadIcons/A.png");
	bButtonTexture = engine.GenerateTexture("Assets/GameAssets/GamepadIcons/B.png");
	xButtonTexture = engine.GenerateTexture("Assets/GameAssets/GamepadIcons/X.png");
	yButtonTexture = engine.GenerateTexture("Assets/GameAssets/GamepadIcons/Y.png");
	spriteTexture = engine.GenerateTexture("Assets/GameAssets/cookie.png");
	/*              End of textures                 */

	// FPS Calculations
	Timer timer;
	timer.setInterval([&]() {
		fps = frameCount;
		frameCount = 0;
	}, 1000);
}

void UWPEngine::MainPage::canvas_Draw(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
	// Update drawing session
	engine.SetDrawingSession(args->DrawingSession);

	// Clear rendering area with specified color
	engine.Clear(Colors::CornflowerBlue);

	// Draw text
	engine.Text(10, 10, "Hello World!", 50, Colors::Black);

	// Draw a textured rectangle
	engine.TexturedRect(aButtonTexture, 10, 90, 30, 30);

	// Draw an unfilled rectangle
	engine.UnfilledRect(10, 140, 100, 50, Colors::Black, 5, 10);

	// Draw n unfilled rectangle
	engine.Rect(120, 140, 100, 50, Colors::Orange);

	// Draw sprite and floor
	engine.TexturedRect(spriteTexture, spriteX, spriteY, 50, 50);
	engine.Rect(objectX, objectY, 200, 20, Colors::Green);

	// Display FPS
	Rect FPSRect = engine.GetStringSizePX("FPS: " + fps, 25, CanvasHorizontalAlignment::Right);
	engine.Text((float)sender->Size.Width - (float)FPSRect.Width, (float)sender->Size.Height - (float)FPSRect.Height, "FPS: " + fps, 25, Colors::Black);

	// Increment frame count for FPS calculations
	frameCount++;
}


void UWPEngine::MainPage::canvas_Update(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedUpdateEventArgs^ args)
{
    // Calculate deltaTime
	deltaTime = engine.CalculateDeltaTime();

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