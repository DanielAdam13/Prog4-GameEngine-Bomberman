#pragma once
#include "Commands/Command.h"

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	class PlayerModeState;

	// Game-specific Command, select currently hovered selectable
	class ConfirmSelectionCommand final : public ge::Command
	{
	public:
		ConfirmSelectionCommand(PlayerModeState& state);

		virtual void Execute(float) override;

	private:
		PlayerModeState& m_GameState;
	};
}