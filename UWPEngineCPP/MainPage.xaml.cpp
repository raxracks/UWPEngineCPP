//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <Engine.h>
#include <timercpp.h>

#define PI 3.14159265

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


// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

Engine engine;

bool fire = false;

int frameCount = 0;
int fps = 0;

float spriteX = 475;
float spriteY = -10;
float objectX = 450;
float objectY = 120;

float MouseX = 0;
float MouseY = 0;

float projectileX = 10;
float projectileY = 500;
float floorX = 0;
float floorY = 600;
float angle = 75;

float H;
float A;
float O;

float dampening = 2;

float gravity = 0.75f;

Engine::EngineTexture spriteTexture;

// Create an HttpClient object.
Windows::Web::Http::HttpClient httpClient;

Timer timer;

MainPage::MainPage()
{
	InitializeComponent();

	engine = Engine(canvas, grid);
}


void UWPEngine::MainPage::canvas_CreateResources(Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::CanvasCreateResourcesEventArgs^ args)
{
	timer.setInterval([&]() {
		fps = frameCount;
		frameCount = 0;
	}, 1000);

	// Add a user-agent header to the GET request.
	//auto headers{ httpClient.DefaultRequestHeaders };

	spriteTexture = engine.GenerateTexture("Assets/GameAssets/cookie.png");
}

void UWPEngine::MainPage::canvas_Draw(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
	// Update drawing session
	engine.SetDrawingSession(args->DrawingSession);

	// Clear rendering area with specified color
	engine.Clear(Colors::CornflowerBlue);

	engine.TexturedRect(spriteTexture, spriteX, spriteY, 32, 32);


	engine.Text(10, 10, fps.ToString() + " FPS", 25, Colors::Black);

	frameCount++;
}


void UWPEngine::MainPage::canvas_Update(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedUpdateEventArgs^ args)
{
	
}

void UWPEngine::MainPage::grid_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	MouseX = e->GetCurrentPoint(this)->Position.X;
	MouseY = e->GetCurrentPoint(this)->Position.Y;
}


void UWPEngine::MainPage::grid_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	
}
