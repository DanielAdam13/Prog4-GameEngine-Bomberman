#pragma once
#include "GameState.h"

#include <memory>

namespace bombGame
{
	class BombermanGame;

	class VictoryState final : public GameState
	{
	public:
		VictoryState(BombermanGame& game, float duration = 6.f);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float deltaTime) override;
		virtual void FixedUpdate(float) override {}

	private:
		float m_VictoryTimer{ 0.f };
		float m_VictoryScreenDuration;

	};
}