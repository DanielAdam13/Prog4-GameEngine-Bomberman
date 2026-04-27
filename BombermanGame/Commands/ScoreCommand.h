#pragma once
#include "Commands/GameObjectCommand.h"

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	// Game-specific Command
	class ScoreCommand final : public ge::GameObjectCommand
	{
	public:
		ScoreCommand(ge::GameObject* commandTarget, int scoreDiff);

		virtual void Execute(float) override;

	private:
		int m_ScoreDiff;
	};
}