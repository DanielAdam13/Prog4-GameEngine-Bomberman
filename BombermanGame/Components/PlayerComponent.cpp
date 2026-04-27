#include "PlayerComponent.h"
#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "GameEvents.h"
#include <cassert>

using namespace bombGame;

PlayerComponent::PlayerComponent(ge::GameObject* owner, float speed)
	:Component::Component(owner),
	m_Speed{ speed }
{
	// Bind health callbacks
	auto* health{ GetOwner()->GetComponent<ge::HealthComponent>()};
	assert(health && "PlayerComponent requires a HealthComponent on the same GameObject");

	health->SetOnTakingDamage([this]() -> void
		{
			m_DamageEvent.NotifyObservers(EventId::PLAYER_LOST_HEALTH, GetOwner());
		});
	health->SetOnDeath([this]() -> void
		{
			m_DamageEvent.NotifyObservers(EventId::PLAYER_DIED, GetOwner());
		});

	// Bind score callbacks
	auto* scoreComp{ GetOwner()->GetComponent<ge::ScoreComponent>() };
	assert(scoreComp && "PlayerComponent requires a ScoreComponent on the same GameObject");

	scoreComp->SetOnScoreChange([this]()
		{
			m_ScoreChangeEvent.NotifyObservers(EventId::PLAYER_SCORE_CHANGED, GetOwner());
		});
}

float PlayerComponent::GetSpeed() const noexcept
{
	return m_Speed;
}

void PlayerComponent::SetSpeed(float newSpeed) noexcept
{
	m_Speed = newSpeed;
}

bool PlayerComponent::IsAlive() const noexcept
{
	auto* healthComp{ GetOwner()->GetComponent<ge::HealthComponent>() };
	return healthComp && !healthComp->IsDead();
}
