#pragma once

#include "Components/Component.h"
#include "EnemyStates/BaseEnemyState.h"
#include "EnemyStates/ChaseState.h"
#include "EnemyStates/WanderState.h"
#include "EnemyStates/RunState.h"

#include <glm/glm.hpp>
#include <vector>
#include <memory>

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

		explicit EnemyComponent(ge::GameObject* owner, float speed = 60.f, float detectionRadius = 200.f);
		~EnemyComponent() override = default;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override;
		void RenderComponent() const override {};

		// States:
		void InitializeStates();
		void TransitionToWander();
		void TransitionToChase();

		// Targets:
		std::vector<ge::GameObject*> GetTargets() const noexcept { return m_Targets; }
		const std::vector<ge::Transform*>& GetTargetTransforms() const noexcept { return m_TargetTransforms; }
		ge::GameObject* GetTargetAt(int index) const noexcept { return m_Targets[index]; }
		ge::Transform* GetOwnerTransform() const noexcept { return m_OwnerTransformRef; }

		glm::vec3 GetMoveDirection() const noexcept { return m_CurrentMoveDirection; }
		float GetSpeed() const noexcept { return m_Speed; }
		float GetDetectionRadius() const noexcept { return m_DetectionRadius; }

		void AddTarget(ge::GameObject* newTarget) noexcept;
		void SetMoveDirection(glm::vec3 direction) noexcept { m_CurrentMoveDirection = direction; }
		void SetSpeed(float newSpeed) noexcept { m_Speed = newSpeed; }

		bool IsAlive() const noexcept;

	private:
		std::vector<ge::GameObject*> m_Targets{}; // Cached ref
		std::vector<ge::Transform*> m_TargetTransforms{}; // Cached ref
		ge::Transform* m_OwnerTransformRef; // Cached ref

		float m_Speed{ 60.f };
		glm::vec3 m_CurrentMoveDirection{};

		float m_DetectionRadius{ 200.f };

		// States:
		std::unique_ptr<ChaseState> m_ChaseState;
		std::unique_ptr<WanderState> m_WanderState;
		EnemyState* m_CurrentState;

	};
}