#pragma once
#include "Component.h"

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

		// Could do nothing if m_OnDeath is not set
		void Die();

		void SetOnTakingDamage(std::function<void()> callback);
		void SetOnDeath(std::function<void()> callback);

	private:
		int m_MaxHealth{};
		int m_CurrentHealth{};

		std::function<void()> m_OnTakingDamage{};
		std::function<void()> m_OnDeath{};

	};
}