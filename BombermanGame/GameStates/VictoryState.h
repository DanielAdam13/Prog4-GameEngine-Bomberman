#pragma once
#include "GameState.h"

#include <memory>

namespace bombGame
{
	class BombermanGame;

	class VictoryState final : public GameState
	{
	public:
		VictoryState(BombermanGame& game);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float deltaTime) override;
		virtual void FixedUpdate(float) override {}

	private:
		static inline float victoryTimer{ 0.f };
		static inline constexpr float victoryScreenDuration{ 3.f };

	};
}