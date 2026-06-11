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

	enum class PlayerMode
	{
		SinglePlayer,
		Coop,
		Versus
	};

	// Stores game info since the player mode was selected and preserves it until game reset/won/lost
	struct GameSession
	{
		int currentStageIndex{ 0 };
		// Set by playermode state, used by gameplay state
		PlayerMode currentPlayerMode{ PlayerMode::SinglePlayer };
		int playerLives{ 4 };
		int totalScore{ 0 };
		std::vector<PowerupType> storedPowerups;
	};

	// Containing logic of the Bomberman Game itself
	class BombermanGame final : public ge::IGameApplication
	{
	public:
		BombermanGame();
		~BombermanGame() = default;
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

		// Only for reading data
		const GameSession& GetCurrentGameSession() const noexcept;
		
		// Game Session modifying methods
		void ClearGameSession();
		void FailStage(int lossScore = 0); // Leave 0 if game is not lost
		void CompleteStage(int score, std::vector<PowerupType> pickedPowerups);
		void SetPlayerMode(PlayerMode mode);

	private:
		GameStateMachine m_GameStateMachine{};

		ge::SoundSystem* m_StoredSoundSystem;
		SoundManager m_BombermanSoundManager{};

		void LoadSound();

		GameSession m_CurrentGameSession{};
	};

	namespace sceneNames
	{
		inline constexpr auto PlayerModeSelection{ "S_PlayerModeSelection" };
		inline constexpr auto MainMenu{ "S_MainMenu" };
		inline constexpr auto Gameplay{ "S_Gameplay" };
		inline constexpr auto GameOver{ "S_GameOver" };
		inline constexpr auto Transition{ "S_Transition" };
		inline constexpr auto Victory{ "S_Victory" };
		inline constexpr auto Loss{ "S_Loss" };
	}
}