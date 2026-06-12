#pragma once
#include "Commands/Command.h"
#include "ISelectableNavigator.h"

namespace bombGame
{
	class GameState;

	// Game-specific Command, select currently hovered selectable
	class ConfirmSelectionCommand final : public ge::Command
	{
	public:
		ConfirmSelectionCommand(ge::ISelectableNavigator* nav);

		virtual void Execute(float) override;

	private:
		ge::ISelectableNavigator* m_Navigator;
	};
}