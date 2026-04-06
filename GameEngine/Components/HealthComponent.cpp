#include "HealthComponent.h"

using namespace ge;

HealthComponent::HealthComponent(ge::GameObject* pOwnerPtr, int maxHealth)
	:ge::Component::Component(pOwnerPtr),
	m_MaxHealth{ maxHealth },
	m_CurrentHealth{ m_MaxHealth }
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

bool HealthComponent::IsDead() const noexcept
{
	return m_CurrentHealth <= 0;
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

void HealthComponent::SetOnDeath(std::function<void()> callback)
{
	m_OnDeath = callback;
}
