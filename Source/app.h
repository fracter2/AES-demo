#pragma once		// TODO Consider making into #ifdef... type include guard (portability, recomended by cpp guidelines)

#include <memory>
#include <string_view>

#include "scene.h"
#include "leaderboard.h"
#include "texture_atlass.h"

class WindowLoader {								// TODO Consider a separate file for this
public:
	WindowLoader() = delete;
	WindowLoader(int screenWidth, int screenHeight, const char* title);
	~WindowLoader();

	WindowLoader(const WindowLoader&) = delete;
	WindowLoader& operator=(const WindowLoader&) = delete;
	WindowLoader(WindowLoader&&) = delete;
	WindowLoader& operator=(WindowLoader&&) = delete;
};

class App {
public:
	explicit App(SceneId sceneId = SceneId::MainMenu);

	void Update() noexcept;
	void Render() const noexcept;
	void QueueTransitionTo(SceneId id) noexcept;

private:
	void ChangeTo(SceneId id);

	static constexpr int screenWidth = 1920;
	static constexpr int screenHeight = 1080;
	static constexpr std::string_view title = { "SPACE INVADERS" };
	WindowLoader windowLoader = { screenWidth, screenHeight, title.data() };
	TextureAtlass resources = {};

	Leaderboard leaderboard;

	std::unique_ptr<Scene> currentScene;			// TODO Consider if this should also be gsl::not_null

	bool transitionQueued = false;
	SceneId queuedId;

	inline bool SceneIsBusy() const noexcept { return inUpdate || inRender; }
	bool inUpdate = false;
	mutable bool inRender = false;
};

