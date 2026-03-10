#pragma once


class App {
public:
	App();
	~App();

	void Update() noexcept;
	void Render() const noexcept;

	App(const App&) = delete;
	App& operator=(const App&) = delete;
	App(App&&) = delete;
	App& operator=(App&&) = delete;
};

