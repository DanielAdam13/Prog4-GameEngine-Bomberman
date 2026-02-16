#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <chrono>

namespace ge
{
	class GameEngine final
	{
	public:
		explicit GameEngine(const std::filesystem::path& dataPath);
		~GameEngine();
		void Run(const std::function<void()>& load);
		void RunOneFrame(const float deltaTime, float& lag);

		GameEngine(const GameEngine& other) = delete;
		GameEngine(GameEngine&& other) = delete;
		GameEngine& operator=(const GameEngine& other) = delete;
		GameEngine& operator=(GameEngine&& other) = delete;

		const float GetFPS() const noexcept { return m_CurrentFPS; }

	private:
		bool m_Quit;
		const float m_FixedTimeStep;
		float m_CurrentFPS;

		void ComputeFPS(float deltaTime, float& fpsTimer, int& frameCount);
	};
}