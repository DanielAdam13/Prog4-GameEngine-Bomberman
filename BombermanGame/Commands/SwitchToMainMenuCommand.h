#pragma once
#include "Commands/Command.h"

namespace bombGame
{
	class BombermanGame;

	// Game-specific Command, calls RequestStateChange from the Game State Machine
	class SwitchToMainMenuCommand final : public ge::Command
	{
	public:
		SwitchToMainMenuCommand(BombermanGame& game);

		virtual void Execute(float) override;

	private:
		BombermanGame& m_BomberGame; // Cached ref to bomber man game
	};
}