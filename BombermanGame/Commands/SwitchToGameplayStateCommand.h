#pragma once
#include "Commands/Command.h"

#include <string>

namespace bombGame
{
	class BombermanGame;

	// Game-specific Command, calls ReuqestStateChange from the Game State Machine
	class SwitchToGameplayCommand final : public ge::Command
	{
	public:
		SwitchToGameplayCommand(const std::string& gameplaySceneName, BombermanGame& game);

		virtual void Execute(float) override;

	private:
		std::string m_GameplaySceneName;
		BombermanGame& m_BomberGame; // Cached ref to bomber man game
	};
}