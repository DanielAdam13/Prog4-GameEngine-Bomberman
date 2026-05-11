#pragma once
#include "Commands/GameObjectCommand.h"

#include <functional>

namespace ge
{
	class GameObject;
	class Texture2D;
}

namespace bombGame
{
	// Game-specific Command, is a Subject for the Lay_Bomb event
	class LayBombCommand final : public ge::GameObjectCommand
	{
	public:
		LayBombCommand(ge::GameObject* commandTarget);

		virtual void Execute(float) override;

	private:
	};
}