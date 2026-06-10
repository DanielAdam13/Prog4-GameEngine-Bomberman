#pragma once
#include "IGameApplication.h"
#include "GameStates/GameStateMachine.h"
#include "SoundManager.h"
#include "PowerupType.h"

#include <memory>
#include <vector>

namespace ge
{
	class SoundSystem;
	class GameObject;
}

namespace bombGame
{
	class SoundManager;

	// Containing logic of the Bomberman Game itself
	class BombermanGame final : public ge::IGameApplication
	{
	public:
		BombermanGame();
		~BombermanGame();
		BombermanGame(const BombermanGame& other) = delete;
		BombermanGame(BombermanGame && other) = delete;
		BombermanGame& operator=(const BombermanGame& other) = delete;
		BombermanGame& operator=(BombermanGame&& other) = delete;

		virtual void Load() override; // Sets Initial state for game state machine
		virtual void Update(float deltaTime) override; // Used only for GAME state machine Update
		virtual void FixedUpdate(float fixedDeltaSec) override; // Used only for GAME state machine FixedUpdate

		SoundManager& GetSoundManager() noexcept;
		ge::SoundSystem* GetStoredSoundSystem() noexcept;
		GameStateMachine& GetStateMachine() noexcept;

		struct GameSession
		{
			int currentStageIndex{ 0 };
			int playerLives{ 4 }; // (shared for both players)
			int totalScore{ 0 }; // (shared for both players)
			std::vector<PowerupType> storedPowerups;
		};

		GameSession& GetCurrentGameSession() noexcept;

	private:
		GameStateMachine m_GameStateMachine{};

		ge::SoundSystem* m_StoredSoundSystem{ nullptr };
		SoundManager m_BombermanSoundManager{};

		void LoadSound();

		GameSession m_CurrentGameSession{};
	};

	namespace sceneNames
	{
		inline constexpr auto MainMenu{ "S_MainMenu" };
		inline constexpr auto Gameplay{ "S_Gameplay" };
		inline constexpr auto GameOver{ "S_GameOver" };
		inline constexpr auto Transition{ "S_Transition" };
		inline constexpr auto Victory{ "S_Victory" };
		inline constexpr auto Loss{ "S_Loss" };
	}
}