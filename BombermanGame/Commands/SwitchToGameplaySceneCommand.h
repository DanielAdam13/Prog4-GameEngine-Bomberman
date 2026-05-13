#pragma once
#include "Commands/Command.h"

#include <functional>
#include <string>

namespace bombGame
{
	// Game-specific Command, is a Subject for the Lay_Bomb event
	class SwitchToGamplayCommand final : public ge::Command
	{
	public:
		SwitchToGamplayCommand(const std::string& gameplaySceneName);

		virtual void Execute(float) override;

	private:
		std::string m_GameplaySceneName;

	};
}