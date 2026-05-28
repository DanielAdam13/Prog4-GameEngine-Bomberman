#pragma once
#include "GameState.h"
#include "LevelBuilder.h"
#include "Camera.h"

#include <memory>

namespace bombGame
{
	class BombermanGame;
	class LevelGrid;

	class GameplayGameState final : public GameState
	{
	public:
		GameplayGameState(BombermanGame& game);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float deltaTime) override;
		virtual void FixedUpdate(float) override {}

	private:
		std::unique_ptr<LevelGrid> m_LevelGrid; // Exists for the whole gameplay state
		std::unique_ptr<ge::Camera> m_GameplayCamera;
	};
}