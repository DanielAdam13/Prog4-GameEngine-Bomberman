#pragma once
#include "GameObjectCommand.h"

namespace ge
{
	class Transform;

	class MoveCommand final : public GameObjectCommand
	{
	public:
		MoveCommand(GameObject* target);
		~MoveCommand() override = default;

		virtual void Execute(float deltaTime) override;

	private:
		void Move(float deltaTime);
		Transform* m_pTargetTransform;

	};
}