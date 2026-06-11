#pragma once
#include "Commands/Command.h"
#include <utility>

namespace bombGame
{
	class GameState;

	// Game-specific Command, hover left command
	class HoverSelectableCommand final : public ge::Command
	{
	public:
		HoverSelectableCommand(GameState* state, std::pair<int,int> delta);

		virtual void Execute(float) override;

	private:
		GameState* m_GameState;
		const std::pair<int, int> m_Delta;
	};
}