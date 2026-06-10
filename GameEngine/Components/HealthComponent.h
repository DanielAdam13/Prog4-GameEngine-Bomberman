#pragma once
#include "Component.h"
#include "ObservableSubject.h"

namespace ge
{
	class GameObject;

	// Engine Component
	class HealthComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every HealthComponent Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 5 };

		HealthComponent(ge::GameObject* pOwnerPtr, int maxHealth);
		virtual ~HealthComponent() override = default;

		virtual void RenderComponent() const override {};
		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override {};

		void TakeDamage(int damageAmount);
		int GetCurrentHealth() const noexcept;
		bool IsDead() const noexcept;
		void Die();

		Subject& GetOnTakingDamageEvent() noexcept { return m_OnTakingDamageEvent; }
		Subject& GetOnDeathEvent() noexcept { return m_OnDeathEvent; }

	private:
		int m_MaxHealth{};
		int m_CurrentHealth{};

		Subject m_OnTakingDamageEvent{};
		Subject m_OnDeathEvent{};
	};
}