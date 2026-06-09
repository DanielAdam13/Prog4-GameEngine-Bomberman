#pragma once
#include "Commands/Command.h"

#include <string>

namespace bombGame
{
	class BombermanGame;

	// Game-specific Command, calls RequestStateChange from the Game State Machine
	class SkipGameplayStageCommand final : public ge::Command
	{
	public:
		SkipGameplayStageCommand(BombermanGame& game);

		virtual void Execute(float) override;

	private:
		BombermanGame& m_BomberGame; // Cached ref to bomber man game
	};
}