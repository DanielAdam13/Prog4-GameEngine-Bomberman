#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <chrono>
#include "Singleton.h"

namespace ge
{
	class GameEngine final : public Singleton<GameEngine>
	{
	public:
		void InitializeEngine(const std::filesystem::path& dataPath);
		
		~GameEngine();
		void Run(const std::function<void()>& load);
		void RunOneFrame(const float deltaTime, float& lag);

		GameEngine(const GameEngine& other) = delete;
		GameEngine(GameEngine&& other) = delete;
		GameEngine& operator=(const GameEngine& other) = delete;
		GameEngine& operator=(GameEngine&& other) = delete;

		float GetFPS() const noexcept { return m_CurrentFPS; }

	private:
		GameEngine() = default;

		// For GetInstance()
		friend class Singleton<GameEngine>;

		bool m_Quit{ false };
		const float m_FixedTimeStep{ 0.02f };
		float m_CurrentFPS{ 0 };

		void ComputeFPS(float deltaTime, float& fpsTimer, int& frameCount);
	};
}