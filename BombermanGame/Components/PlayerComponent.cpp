#include "PlayerComponent.h"
#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "GameEvents.h"

#include "Components/Colliders.h"
#include "EngineEvents.h"

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
			m_DamageEvent.NotifyObservers(GameEventId::PLAYER_LOST_HEALTH, GetOwner());
		});
	health->SetOnDeath([this]() -> void
		{
			m_DamageEvent.NotifyObservers(GameEventId::PLAYER_DIED, GetOwner());
		});

	// Bind score callbacks
	auto* scoreComp{ GetOwner()->GetComponent<ge::ScoreComponent>() };
	assert(scoreComp && "PlayerComponent requires a ScoreComponent on the same GameObject");

	scoreComp->SetOnScoreChange([this]()
		{
			m_ScoreChangeEvent.NotifyObservers(GameEventId::PLAYER_SCORE_CHANGED, GetOwner());
		});

	auto* colliderComp{ GetOwner()->GetComponent<ge::BoxCollider>() };
	assert(colliderComp && "PlayerComponent requires a BoxCollider on the same GameObject");
	colliderComp->GetOnCollisionEnterEvent().AddObserver(this);
	colliderComp->GetOnCollisionExitEvent().AddObserver(this);
}

bombGame::PlayerComponent::~PlayerComponent()
{
	if (auto* h = GetOwner()->GetComponent<ge::HealthComponent>())
	{
		h->SetOnTakingDamage(nullptr);
		h->SetOnDeath(nullptr);
	}
	if (auto* s = GetOwner()->GetComponent<ge::ScoreComponent>()) 
	{
		s->SetOnScoreChange(nullptr);
	}
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

void bombGame::PlayerComponent::Notify(int eventId, ge::GameObject* other)
{
	if (!other) 
		return;

	ge::Collider* otherColl{ other->GetComponent<ge::BoxCollider>() };
	if(!otherColl)
		otherColl = other->GetComponent<ge::CircleCollider>();
	if (!otherColl)
		return;

	const auto& tag{ otherColl->GetLayerTag() };

	switch (eventId)
	{
	case ge::EngineEventId::COLLISION_ENTER:
		OnCollisionEnter(other, tag);
		break;
	case ge::EngineEventId::COLLISION_EXIT:
		OnCollisionExit(other, tag);
		break;
	}
}

void bombGame::PlayerComponent::OnCollisionEnter(ge::GameObject*, const ge::CollisionLayerTag& tag)
{
	if (tag == "Wall")
	{
		m_Speed = 0.f;
	}
	else if (tag == "Enemy")
	{
		// Take damage
		if (auto* health = GetOwner()->GetComponent<ge::HealthComponent>())
			health->TakeDamage(1);
	}
	else if (tag == "Explosion")
	{
		// Take damage
		if (auto* health = GetOwner()->GetComponent<ge::HealthComponent>())
			health->TakeDamage(1);
	}
	else if (tag == "Powerup")
	{
		// Collect — call an event or call into the powerup's component.
	}
}

void bombGame::PlayerComponent::OnCollisionExit(ge::GameObject*, const ge::CollisionLayerTag&)
{
	// clean for now
}
