#include "HealthComponent.h"

using namespace ge;

HealthComponent::HealthComponent(GameObject* pOwnerPtr, int maxHealth)
	:Component::Component(pOwnerPtr),
	m_MaxHealth{ maxHealth },
	m_CurrentHealth{ m_MaxHealth },
	m_IsDead{ false }
{
}

void HealthComponent::TakeDamage(int damageAmount)
{
	m_CurrentHealth = std::max(0, m_CurrentHealth - damageAmount);

	if (m_OnTakingDamage)
		m_OnTakingDamage();

	if (m_CurrentHealth <= 0)
		Die(); // Could do nothing if m_OnDeath is not set
}

int HealthComponent::GetCurrentHealth() const noexcept
{
	return m_CurrentHealth;
}

void HealthComponent::Die()
{
	if (m_OnDeath) // If there is a bound function
		m_OnDeath(); // Execute
}

void HealthComponent::SetOnTakingDamage(std::function<void()> callback)
{
	m_OnTakingDamage = callback;
}

void HealthComponent::SetOnDeathCallback(std::function<void()> callback)
{
	m_OnDeath = callback;
}
