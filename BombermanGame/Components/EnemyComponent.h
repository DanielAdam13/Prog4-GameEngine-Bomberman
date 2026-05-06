#pragma once

#include "Components/Component.h"

#include <glm/glm.hpp>
#include <vector>

namespace ge
{
	class GameObject;
	class Transform;
}

namespace bombGame
{
	// Game-specific Component
	class EnemyComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every PlayerComponent Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 13 };

		explicit EnemyComponent(ge::GameObject* owner, float speed = 6.f, float detectionRadius = 200.f);
		~EnemyComponent() override = default;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override {};
		void RenderComponent() const override {};

		std::vector<ge::GameObject*> GetTargets() const noexcept { return m_Targets; }
		ge::GameObject* GetTargetAt(int index) const noexcept { return m_Targets[index]; }
		glm::vec3 GetMoveDirection() const noexcept { return m_CurrentMoveDirection; }
		float GetSpeed() const noexcept { return m_Speed; }
		float GetDetectionRadius() const noexcept { return m_DetectionRadius; }

		void AddTarget(ge::GameObject* newTarget) noexcept;
		void SetMoveDirection(glm::vec3 direction) noexcept { m_CurrentMoveDirection = direction; }
		void SetSpeed(float newSpeed) noexcept { m_Speed = newSpeed; }

		bool IsAlive() const noexcept;

	private:
		std::vector<ge::GameObject*> m_Targets{ nullptr };
		ge::Transform* m_OwnerTransformRef; // reference

		float m_Speed{ 200.f };
		glm::vec3 m_CurrentMoveDirection;

		float m_DetectionRadius{ 200.f };

	};
}