#pragma once
#include "Commands/Command.h"

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	class PlayerModeState;

	// Game-specific Command, hover left command
	class HoverSelectableCommand final : public ge::Command
	{
	public:
		HoverSelectableCommand(PlayerModeState& state, int delta);

		virtual void Execute(float) override;

	private:
		PlayerModeState& m_GameState;
		const int m_Delta;
	};
}