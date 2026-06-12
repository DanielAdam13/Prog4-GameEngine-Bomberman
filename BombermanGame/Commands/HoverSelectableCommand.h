#pragma once
#include "Commands/Command.h"
#include "ISelectableNavigator.h"

#include <utility>

namespace bombGame
{
	class GameState;

	// Game-specific Command, hover left command
	class HoverSelectableCommand final : public ge::Command
	{
	public:
		HoverSelectableCommand(ge::ISelectableNavigator* nav, std::pair<int,int> delta);

		virtual void Execute(float) override;

	private:
		ge::ISelectableNavigator* m_Navigator;
		const std::pair<int, int> m_Delta;
	};
}