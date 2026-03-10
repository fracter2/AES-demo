#include "app.h"
#include <cassert>
#include <functional>

#include "game.h"
#include "main_menu.h"
#include "end_screen.h"
#include "highscore_scene.h"
#include "something_went_wrong_scene.h"

App::App(SceneId sceneId) 
	: queuedId(sceneId)
	, leaderboard(GetExampleLeaderboard())
{
	SetTargetFPS(60);

	ChangeTo(sceneId);
}

void App::QueueTransitionTo(SceneId id) noexcept 
{
	queuedId = id;
	transitionQueued = true;
}

void App::ChangeTo(SceneId id) 
{
	assert(!SceneIsBusy());
	assert(SceneId::Min < id && id < SceneId::Max);

	std::function<void(SceneId)> transitionFunc = std::bind_front(&App::QueueTransitionTo, this);		
	
	// TODO Consider making this a non-owning raw ptr, pointing to an already stored scene of each. Re-making each scene on-call
	switch (id) {
	case SceneId::MainMenu:  { currentScene = std::unique_ptr<Scene>(new MainMenu(		transitionFunc)); break; }
	case SceneId::Game:		 { currentScene = std::unique_ptr<Scene>(new Game(			transitionFunc, leaderboard, resources)); break; }
	case SceneId::Highscore: { currentScene = std::unique_ptr<Scene>(new HighscoreScene(transitionFunc, leaderboard)); break; }
	case SceneId::EndScreen: { currentScene = std::unique_ptr<Scene>(new EndScreen(		transitionFunc, leaderboard)); break; }
	case SceneId::SomethingWentWrong: { currentScene = std::unique_ptr<Scene>(new SomethingWentWrongScene(transitionFunc, std::current_exception())); break; }
	}
}

void App::Update() noexcept 
{
	try {
		inUpdate = true;
		currentScene->Update();
		inUpdate = false;

		while (transitionQueued) {
			transitionQueued = false;
			ChangeTo(queuedId);
		}
	}
	catch (...) {
		inUpdate = false;
		transitionQueued = false;
		ChangeTo(SceneId::SomethingWentWrong);		// If this returns std::bad_alloc (since it creates new scene), RIP. main.cpp will catch.
	}
}

void App::Render() const noexcept 
{
	BeginDrawing();	
	ClearBackground(BLACK);

	inRender = true;
	currentScene->Render();
	inRender = false;

	EndDrawing();
}

// WindowLoader

WindowLoader::WindowLoader(int screenWidth, int screenHeight, const char* title)		// NOTE std::string_view doesn't work here because raylib wants a c_str pointer
{
	InitWindow(screenWidth, screenHeight, title);
}

WindowLoader::~WindowLoader()
{
	CloseWindow();
}
