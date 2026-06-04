#pragma once
#include "Commands/GameObjectCommand.h"

namespace bombGame
{
	class BombLayerComponent;

	class RemoteBombDetonateCommand final : public ge::GameObjectCommand
	{
	public:
		RemoteBombDetonateCommand(ge::GameObject* commandTarget);

		virtual void Execute(float) override;

	private:

	};
}
