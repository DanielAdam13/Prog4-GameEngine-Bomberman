#pragma once
#include <string>
#include <functional>
#include <filesystem>

namespace ge
{
	class GameEngine final
	{
	public:
		explicit GameEngine(const std::filesystem::path& dataPath);
		~GameEngine();
		void Run(const std::function<void()>& load);
		void RunOneFrame();

		GameEngine(const GameEngine& other) = delete;
		GameEngine(GameEngine&& other) = delete;
		GameEngine& operator=(const GameEngine& other) = delete;
		GameEngine& operator=(GameEngine&& other) = delete;

	private:
		bool m_quit{ false };
	
	};
}