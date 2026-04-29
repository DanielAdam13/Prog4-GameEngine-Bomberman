#pragma once

namespace ge
{
	class SoundSystem;
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

	private:
		static ge::SoundSystem* StoredSoundSystem;
		static SoundManager BombermanSoundManager;

		static void InitializeFirstScene();
		static void InitializeMainPlayersScene();

		
	};
}