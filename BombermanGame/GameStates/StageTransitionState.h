#pragma once
#include "GameState.h"

#include <memory>

namespace bombGame
{
	class BombermanGame;

	class StageTransitionState final : public GameState
	{
	public:
		StageTransitionState(BombermanGame& game, float duration = 2.7f);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float deltaTime) override;
		virtual void FixedUpdate(float) override {}

	private:
		int m_StageIndex;

		float m_Timer{ 0.f };
		float m_TransitionDuration;

	};
}