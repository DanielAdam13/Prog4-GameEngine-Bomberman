#include "HealthComponent.h"
#include "EngineEvents.h"

using namespace ge;

HealthComponent::HealthComponent(ge::GameObject* pOwnerPtr, int maxHealth)
	:ge::Component::Component(pOwnerPtr),
	m_MaxHealth{ maxHealth },
	m_CurrentHealth{ m_MaxHealth }
{
}

void HealthComponent::TakeDamage(int damageAmount)
{
	if (m_CurrentHealth <= 0) 
		return;

	m_CurrentHealth = std::max(0, m_CurrentHealth - damageAmount);

	m_OnTakingDamageEvent.NotifyObservers(EngineEventId::HEALTH_TAKING_DAMAGE, GetOwner());

	if (m_CurrentHealth <= 0)
		Die();
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
	m_OnDeathEvent.NotifyObservers(EngineEventId::HEALTH_DIED, GetOwner());
}
