#pragma once
#include "Singleton.h"

#include <string>
#include <functional>
#include <filesystem>
#include <chrono>

namespace ge
{
	class IGameApplication;

	class GameEngine final : public Singleton<GameEngine>
	{
	public:
		void InitializeEngine(const std::filesystem::path& dataPath);
		
		~GameEngine();
		void Run(IGameApplication& app);
		void RunOneFrame(IGameApplication& app);

		GameEngine(const GameEngine& other) = delete;
		GameEngine(GameEngine&& other) = delete;
		GameEngine& operator=(const GameEngine& other) = delete;
		GameEngine& operator=(GameEngine&& other) = delete;

	private:
		GameEngine() = default;

		friend class Singleton<GameEngine>;

		bool m_Quit{ false };
		const float m_FixedTimeStep{ 0.02f };

		static constexpr std::chrono::duration<float> targetFrameRate{ std::chrono::duration<float>(1.f / 65.f) };

		float m_DeltaTime{ 0.f };
		float m_FrameLag{ 0.f };

		std::chrono::high_resolution_clock::time_point m_LastTime{};
		bool m_FirstFrame{ true };
	};
}