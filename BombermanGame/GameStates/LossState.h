#pragma once
#include "GameState.h"

#include <memory>

namespace bombGame
{
	class BombermanGame;

	class LossState final : public GameState
	{
	public:
		LossState(BombermanGame& game, float duration = 4.f);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float deltaTime) override;
		virtual void FixedUpdate(float) override {}

	private:
		float m_LossTimer{ 0.f };
		float m_LossScreenDuration;

	};
}