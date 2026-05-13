#pragma once
#include "IGameApplication.h"

#include <memory>

namespace ge
{
	class SoundSystem;
	class GameObject;
}

namespace bombGame
{
	class SoundManager;

	// Fully static singleton class containing logic of the Bomberman Game itself
	class BombermanGame final : public ge::IGameApplication
	{
	public:
		BombermanGame();
		~BombermanGame();
		BombermanGame(const BombermanGame& other) = delete;
		BombermanGame(BombermanGame && other) = delete;
		BombermanGame& operator=(const BombermanGame& other) = delete;
		BombermanGame& operator=(BombermanGame&& other) = delete;

		virtual void Load() override;
		virtual void Update(float) override;
		virtual void FixedUpdate(float) override {};

		static SoundManager& GetSoundManager() noexcept;

	private:
		static void LoadSound();
		static void LoadScenes();

		static ge::SoundSystem* StoredSoundSystem;
		static SoundManager BombermanSoundManager;

		static void InitializeMainMenuScene();
		static void InitializeGameplayScene();

		static float CurrentBombExplosion;
		static ge::GameObject CurrentBombTemplate;
	};

	namespace sceneNames
	{
		inline constexpr auto MainMenu{ "MainMenu" };
		inline constexpr auto Gameplay{ "Gameplay" };
		inline constexpr auto GameOver{ "GameOver" };
	}
}