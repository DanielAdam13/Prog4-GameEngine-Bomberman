#pragma once
#include "Commands/Command.h"

#include <string>

namespace bombGame
{
	class BombermanGame;

	// Game-specific Command, calls RequestStateChange from the Game State Machine
	class ToggleMuteCommand final : public ge::Command
	{
	public:
		ToggleMuteCommand(BombermanGame& game);

		virtual void Execute(float) override;

	private:
		BombermanGame& m_Game; // Cached ref
	};
}