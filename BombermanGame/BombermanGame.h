#pragma once

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
	class BombermanGame final
	{
	public:
		BombermanGame();
		~BombermanGame();
		BombermanGame(const BombermanGame& other) = delete;
		BombermanGame(BombermanGame && other) = delete;
		BombermanGame& operator=(const BombermanGame& other) = delete;
		BombermanGame& operator=(BombermanGame&& other) = delete;

		static void LoadGame();
		static void LoadSound();
		static void LoadScenes();

		static SoundManager& GetSoundManager() noexcept;

	private:
		static ge::SoundSystem* StoredSoundSystem;
		static SoundManager BombermanSoundManager;

		//static void InitializeFirstScene();
		static void InitializeMainGameplayScene();

		static float CurrentBombExplosion;
		static ge::GameObject CurrentBombTemplate;
	};
}