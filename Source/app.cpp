#include "app.h"
#include "raylib.h"

App::App() 
{
	static constexpr int screenWidth = 1920;
	static constexpr int screenHeight = 1080;
	InitWindow(screenWidth, screenHeight, "AES DEMO");
	SetTargetFPS(60);
}

App::~App()
{
	CloseWindow();
}

void App::Update() noexcept 
{
	// TODO
}

void App::Render() const noexcept 
{
	BeginDrawing();	
	ClearBackground(BLACK);

	// TODO

	EndDrawing();
}

void App::Main()
{
	while (!WindowShouldClose()) {
		Update();
		Render();
	}
}




