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
			int currentStageIndex{ 1 };
			int playerLives{ 2 };
			int totalScore{ 0 };
			std::vector<PowerupType> storedPowerups;
		};

		GameSession GetCurrentGameSession() const noexcept;

	private:
		std::unique_ptr<GameStateMachine> m_GameStateMachine{};

		ge::SoundSystem* m_StoredSoundSystem{ nullptr };
		SoundManager m_BombermanSoundManager{};

		void LoadSound();

		GameSession m_CurrentGameSession{};
	};

	namespace sceneNames
	{
		inline constexpr auto MainMenu{ "MainMenu" };
		inline constexpr auto Gameplay{ "Gameplay" };
		inline constexpr auto GameOver{ "GameOver" };
	}
}