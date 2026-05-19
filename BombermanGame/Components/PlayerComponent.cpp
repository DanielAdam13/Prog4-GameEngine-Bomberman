#include "PlayerComponent.h"
#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/Transform.h"
#include "GameEvents.h"
#include "Components/AnimatorComponent.h"

#include "Components/Colliders.h"
#include "CollisionSystem.h"
#include "EngineEvents.h"

#include <cassert>
#include <span>

using namespace bombGame;

PlayerComponent::PlayerComponent(ge::GameObject* owner, float speed)
	:Component::Component(owner),
	m_Speed{ speed },
	m_CachedOwnerTransform{ owner->GetComponent<ge::Transform>() }
{
	// Bind health callbacks
	m_CachedHealthComp = GetOwner()->GetComponent<ge::HealthComponent>();
	assert(m_CachedHealthComp && "PlayerComponent requires a HealthComponent on the same GameObject");

	m_CachedHealthComp->SetOnTakingDamage([this]() -> void
		{
			m_DamageEvent.NotifyObservers(GameEventId::PLAYER_LOST_HEALTH, GetOwner());
		});
	m_CachedHealthComp->SetOnDeath([this]() -> void
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

	m_CachedBoxCollider = GetOwner()->GetComponent<ge::BoxCollider>();
	assert(m_CachedBoxCollider && "PlayerComponent requires a BoxCollider on the same GameObject");
	m_CachedBoxCollider->GetOnCollisionEnterEvent().AddObserver(this);
	m_CachedBoxCollider->GetOnCollisionExitEvent().AddObserver(this);

	m_CachedAnimator = GetOwner()->GetComponent<ge::AnimatorComponent>();
	assert(m_CachedAnimator && "PlayerComponent requires an Animator Component on the same GameObject");

	m_CachedAnimator->Play("idle");
}

bombGame::PlayerComponent::~PlayerComponent()
{
	if (m_CachedHealthComp)
	{
		m_CachedHealthComp->SetOnTakingDamage(nullptr);
		m_CachedHealthComp->SetOnDeath(nullptr);
	}
	if (auto* s = GetOwner()->GetComponent<ge::ScoreComponent>()) 
	{
		s->SetOnScoreChange(nullptr);
	}

	/*m_CachedBoxCollider->GetOnCollisionEnterEvent().RemoveObserver(this);
	m_CachedBoxCollider->GetOnCollisionExitEvent().RemoveObserver(this);*/
}

void bombGame::PlayerComponent::UpdateComponent(float)
{
	if (!IsAlive()) 
	{
		m_CachedAnimator->Play("death");
		m_MovedThisFrame = false;
		return;
	}
	// -------------------------------------
	// Play Movement Animations
	// -------------------------------------
	if (m_MovedThisFrame)
	{
		if (m_LastMoveDir.y < 0.f)
			m_CachedAnimator->Play("walk_up");
		else if (m_LastMoveDir.y > 0.f)
			m_CachedAnimator->Play("walk_down");
		else if (m_LastMoveDir.x < 0.f)
			m_CachedAnimator->Play("walk_left");
		else if (m_LastMoveDir.x > 0.f)
			m_CachedAnimator->Play("walk_right");
	}
	else
	{
		m_CachedAnimator->Play("idle");
	}

	m_MovedThisFrame = false;
}

void bombGame::PlayerComponent::TryMove(const glm::vec3& direction, float deltaTime)
{
	if (!IsAlive())
		return;

	// Try Move, for a Grid based game ->
	// 1. Build a "hyphotetical" box collider in the "future" poisition
	// 2. Predict if this box collides with a specific tag
	// 3. Stop movement for axis if it does collide

	const auto currentPos{ m_CachedOwnerTransform->GetWorldPosition() };
	const auto desiredPos{ currentPos + direction * m_Speed * deltaTime };

	// If it wouldn't overlap, just move
	if (!WouldOverlapWall(desiredPos))
	{
		m_CachedOwnerTransform->SetLocalPosition(desiredPos);

		m_MovedThisFrame = true;
		m_LastMoveDir = direction;

		return;
	}

	// Same check but only for x
	const glm::vec3 xOnly{ desiredPos.x, currentPos.y, currentPos.z };
	if (!WouldOverlapWall(xOnly)) 
	{
		m_CachedOwnerTransform->SetLocalPosition(xOnly);
		return;
	}

	// Same check but only for y
	const glm::vec3 yOnly{ currentPos.x, desiredPos.y, currentPos.z };
	if (!WouldOverlapWall(yOnly))
	{
		m_CachedOwnerTransform->SetLocalPosition(yOnly);
		return;
	}

	// Else if blocked for x AND y -> don't move at all
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
	return !m_CachedHealthComp->IsDead();
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
	case ge::EngineEventId::ANIMATION_FINISHED:
		// This for after death animation finished
		// do nothing for now
		/*if (!IsAlive())
		{

		}*/
		break;
	default:
		break;
	}
}

void bombGame::PlayerComponent::OnCollisionEnter(ge::GameObject*, const ge::CollisionLayerTag& tag)
{
	if (tag == "Enemy" || tag == "Explosion")
	{
		// Take damage
		if (m_CachedHealthComp)
			m_CachedHealthComp->TakeDamage(1);
	}
	else if (tag == "Powerup")
	{
		// Collect —> call an event or call into the powerup's component.
	}
}

void bombGame::PlayerComponent::OnCollisionExit(ge::GameObject*, const ge::CollisionLayerTag&)
{
	// clean for now
}

bool bombGame::PlayerComponent::WouldOverlapWall(const glm::vec3& worldPos) const
{
	// Build and check hyphotetical NEW collider
	const auto boxBounds{ m_CachedBoxCollider->GetBoundsAt(worldPos) };

	return ge::CollisionSystem::GetInstance().AnyOverlapWithTags(boxBounds, { "Wall" });
}
