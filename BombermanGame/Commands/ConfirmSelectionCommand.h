#pragma once
#include "Commands/Command.h"

namespace bombGame
{
	class GameState;

	// Game-specific Command, select currently hovered selectable
	class ConfirmSelectionCommand final : public ge::Command
	{
	public:
		ConfirmSelectionCommand(GameState* state);

		virtual void Execute(float) override;

	private:
		GameState* m_GameState;
	};
}