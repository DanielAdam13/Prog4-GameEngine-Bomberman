#pragma once
#include "Components/Component.h"
#include "ObservableSubject.h"
#include <glm/glm.hpp>

namespace bombGame
{
	// Game-specific Component
	class PlayerComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every PlayerComponent Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 7 };

		explicit PlayerComponent(ge::GameObject* owner, float speed = 120.f);
		~PlayerComponent() override = default;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float)      override {}
		void RenderComponent() const     override {}

		float GetSpeed() const noexcept;
		void SetSpeed(float newSpeed) noexcept;

		bool IsAlive() const noexcept;

		ge::Subject& GetDamageEvent() { return m_DamageEvent; }
		ge::Subject& GetDeadEvent() { return m_DeadEvent; }
		ge::Subject& GetScoreChangeEvent() { return m_ScoreChangeEvent; }

	private:
		float m_Speed;

		ge::Subject m_DamageEvent;
		ge::Subject m_DeadEvent;
		ge::Subject m_ScoreChangeEvent;
	};
}