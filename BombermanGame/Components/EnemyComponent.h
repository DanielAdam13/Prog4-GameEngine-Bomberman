#pragma once

#include "Components/Component.h"
#include "EnemyStates/BaseEnemyState.h"
#include "EnemyStates/ChaseState.h"
#include "EnemyStates/WanderState.h"
#include "Observer.h"
#include "Components/Colliders.h"
#include "Components/AnimatorComponent.h"
#include "ObservableSubject.h"

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <optional>

namespace ge
{
	class GameObject;
	class Transform;
	class HealthComponent;
}

namespace bombGame
{
	class LevelGrid;

	// Game-specific Component
	// TYPED Object - Type Object is the Enemy Archetype, used in enemy creation in Spawn Utils
	class EnemyComponent final : public ge::Component, public ge::IObserver
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every PlayerComponent Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 6 };

		explicit EnemyComponent(ge::GameObject* owner, LevelGrid* levelGrid, float speed, float detectionRadius, int scoreValue);
		~EnemyComponent() override = default;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override;
		void RenderComponent() const override {};

		void InitializeStates();

		// Targets:
		const std::vector<ge::GameObject*>& GetTargets() const noexcept { return m_Targets; }
		const std::vector<ge::Transform*>& GetTargetTransforms() const noexcept { return m_TargetTransforms; }
		ge::GameObject* GetTargetAt(int index) const noexcept { return m_Targets[index]; }
		ge::Transform* GetOwnerTransform() const noexcept { return m_OwnerTransformRef; }
		glm::vec3 GetMoveDirection() const noexcept { return m_CurrentMoveDirection; }
		float GetSpeed() const noexcept { return m_Speed; }
		float GetDetectionRadius() const noexcept { return m_DetectionRadius; }
		int GetScoreValue() const noexcept { return m_ScoreValue; }
		LevelGrid* GetLevelGrid() const noexcept { return m_LevelGrid; } // For states

		bool IsAlive() const noexcept;

		void AddTarget(ge::GameObject* newTarget) noexcept;

		virtual void Notify(int eventId, ge::GameObject* other) override;
		ge::Subject& GetDeadEvent() noexcept;

	private:
		std::vector<ge::GameObject*> m_Targets{}; // Cached ref
		std::vector<ge::Transform*> m_TargetTransforms{}; // Cached ref

		ge::Transform* m_OwnerTransformRef; // Cached ref
		ge::HealthComponent* m_CachedHealthComp; // Cached ref
		ge::BoxCollider* m_CachedBoxCollider; // Cached ref
		ge::AnimatorComponent* m_CachedAnimator; // Cached ref

		LevelGrid* m_LevelGrid{ nullptr }; // Ref

		// Movement related data:
		const float m_Speed;
		const float m_DetectionRadius;
		const int m_ScoreValue;
		glm::vec3 m_CurrentMoveDirection{};

		void PickNextTileTarget();
		bool m_HasTileTarget{ false };
		std::optional<glm::vec2> m_TargetTileCenter;
		bool HasArrivedAtTargetTile() const;

		// States:
		std::unique_ptr<EnemyState> m_CurrentState;
		std::unique_ptr<EnemyState> m_PendingState;
		bool m_TransitionPending{ false };

		ge::Subject m_DeadEvent;

		void OnCollisionEnter(ge::GameObject* other, const ge::CollisionLayerTag& tag);

		void ApplyMovement(float deltaTime);
		void UpdateAnimationLogic();

	};
}