#pragma once
#include "Components/Component.h"
#include "ObservableSubject.h"
#include "Observer.h"
#include "Components/Colliders.h"

#include <glm/glm.hpp>

namespace ge
{
	class Transform;
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
		~PlayerComponent() override;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override {}
		void RenderComponent() const override {}

		void TryMove(const glm::vec3& direction, float deltaTime);

		float GetSpeed() const noexcept;
		void SetSpeed(float newSpeed) noexcept;

		bool IsAlive() const noexcept;

		ge::Subject& GetDamageEvent() { return m_DamageEvent; }
		ge::Subject& GetDeadEvent() { return m_DeadEvent; }
		ge::Subject& GetScoreChangeEvent() { return m_ScoreChangeEvent; }

		virtual void Notify(int eventId, ge::GameObject* other) override;

	private:
		float m_Speed;

		ge::Subject m_DamageEvent;
		ge::Subject m_DeadEvent;
		ge::Subject m_ScoreChangeEvent;

		void OnCollisionEnter(ge::GameObject* other, const ge::CollisionLayerTag& tag);
		void OnCollisionExit(ge::GameObject* other, const ge::CollisionLayerTag& tag);

		// Helper for tile-based collision check
		bool WouldOverlapWall(const glm::vec3& worldPos) const;

		ge::Transform* m_CachedOwnerTransform;
		ge::BoxCollider* m_CachedBoxCollider;
	};
}