#pragma once
#include "Components/Component.h"
#include "ObservableSubject.h"
#include "Observer.h"
#include "Components/Colliders.h"

#include <glm/glm.hpp>

namespace ge
{
	class Transform;
	class HealthComponent;
	class ScoreComponent;
	class AnimatorComponent;
}

namespace bombGame
{
	// Game-specific Component
	class PlayerComponent final : public ge::Component, public ge::IObserver
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every PlayerComponent Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 7 };

		explicit PlayerComponent(ge::GameObject* owner, float speed = 120.f);
		~PlayerComponent() override; // does nothing, look at implementation for details
		PlayerComponent(const PlayerComponent&) = delete;
		PlayerComponent(PlayerComponent&&) = delete;
		PlayerComponent& operator=(const PlayerComponent&) = delete;
		PlayerComponent& operator=(PlayerComponent&&) = delete;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override;
		void RenderComponent() const override {}

		void TryMove(const glm::vec3& direction, float deltaTime);

		float GetSpeed() const noexcept;
		void SetSpeed(float newSpeed) noexcept;
		bool IsAlive() const noexcept;

		// Used for Bomb-Laying, for more specific drop location
		glm::vec3 GetPlayerMidPoint() const noexcept;

		ge::Subject& GetDamageEvent() { return m_DamageEvent; }
		ge::Subject& GetDeadEvent() { return m_DeadEvent; }
		ge::Subject& GetScoreChangeEvent() { return m_ScoreChangeEvent; }
		ge::Subject& GetMovedHorEvent() { return m_MovedHorEvent; }
		ge::Subject& GetMovedVertEvent() { return m_MovedVertEvent; }
		ge::Subject& GetPowerupPickedUpEvent() { return m_PowerupPickedUpEvent; }

		virtual void Notify(int eventId, ge::GameObject* other) override;

	private:
		float m_Speed;

		ge::Subject m_DamageEvent;
		ge::Subject m_DeadEvent;
		ge::Subject m_ScoreChangeEvent;
		ge::Subject m_MovedHorEvent;
		ge::Subject m_MovedVertEvent;
		ge::Subject m_PowerupPickedUpEvent;

		void OnCollisionEnter(ge::GameObject* other, const ge::CollisionLayerTag& tag);

		// Helper for tile-based collision check
		bool WouldOverlapWall(const glm::vec3& worldPos) const;

		ge::Transform* m_CachedOwnerTransform;
		ge::HealthComponent* m_CachedHealthComp;
		ge::ScoreComponent* m_CachedScoreComp;
		ge::BoxCollider* m_CachedBoxCollider;
		ge::AnimatorComponent* m_CachedAnimator;

		bool m_MovedThisFrame{ false }; // Flag for animation logic
		glm::vec3 m_LastMoveDir{ 0, 0, 0 }; // for animation logic

		void UpdateAnimationLogic();

		float m_StepTimer{ 0.f };
		static constexpr float m_StepInterval{ 0.28f };
		void UpdateMovementSound(float deltaTime);
	};
}