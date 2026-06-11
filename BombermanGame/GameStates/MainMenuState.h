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

		virtual std::unique_ptr<GameState> Update(float) override { return nullptr; }
		virtual void FixedUpdate(float) override {}

	private:
	};
}