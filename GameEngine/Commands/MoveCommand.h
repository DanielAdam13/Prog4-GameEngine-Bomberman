#pragma once
#include "GameObjectCommand.h"
#include <glm/glm.hpp>
#include <functional>

namespace ge
{
	class GameObject;
	class Transform;

	// Engine Command
	class MoveCommand final : public GameObjectCommand
	{
	public:
		MoveCommand(GameObject* target, const glm::vec3& startingDirection, float moveSpeed = 80.f);
		MoveCommand(GameObject* target, const glm::vec3& startingDirection, std::function<float()> speedFunc);
		~MoveCommand() override = default;

		virtual void Execute(float deltaTime) override;

		void SetSpeed(float newSpeed);
		float GetSpeed() const noexcept;

	private:
		void Move(float deltaTime);
		Transform* m_pTargetTransform;

		const glm::vec3 m_ConstantMoveDirection;
		float m_MoveSpeed;
		std::function<float()> m_SpeedFunc{};

	};
}