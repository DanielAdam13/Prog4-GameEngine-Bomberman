#pragma once
#include "GameObjectCommand.h"
#include <glm/glm.hpp>

namespace ge
{
	class Transform;

	// Engine Command
	class MoveStickCommand final : public GameObjectCommand
	{
	public:
		MoveStickCommand(GameObject* target, float speed = 80.f);

		virtual void Execute(float deltaTime) override;

	private:
		void MoveController(float deltaTime);
		Transform* m_pTargetTransform;

		float m_MoveSpeed;
	};
}
