#pragma once
#include "GameState.h"

#include <memory>

namespace bombGame
{
	class BombermanGame;

	class MainMenuGameState final : public GameState
	{
	public:
		MainMenuGameState(BombermanGame& game);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float deltaTime) override;
		virtual void FixedUpdate(float) override {}

	private:
	};
}