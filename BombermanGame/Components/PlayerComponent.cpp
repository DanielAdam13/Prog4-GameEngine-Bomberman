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
	// Subscribe to Health
	m_CachedHealthComp = GetOwner()->GetComponent<ge::HealthComponent>();
	assert(m_CachedHealthComp && "PlayerComponent requires a HealthComponent on the same GameObject");
	m_CachedHealthComp->GetOnTakingDamageEvent().AddObserver(this);
	m_CachedHealthComp->GetOnDeathEvent().AddObserver(this);

	// Subscribe to Score
	auto* scoreComp{ GetOwner()->GetComponent<ge::ScoreComponent>() };
	assert(scoreComp && "PlayerComponent requires a ScoreComponent on the same GameObject");
	scoreComp->GetOnScoreChangedEvent().AddObserver(this);

	// Subscribe to Box Collider
	m_CachedBoxCollider = GetOwner()->GetComponent<ge::BoxCollider>();
	assert(m_CachedBoxCollider && "PlayerComponent requires a BoxCollider on the same GameObject");
	m_CachedBoxCollider->GetOnCollisionEnterEvent().AddObserver(this);
	//m_CachedBoxCollider->GetOnCollisionExitEvent().AddObserver(this);

	// Subscribe to Animator
	m_CachedAnimator = GetOwner()->GetComponent<ge::AnimatorComponent>();
	assert(m_CachedAnimator && "PlayerComponent requires an Animator Component on the same GameObject");
	m_CachedAnimator->GetOnAnimationFinishedEvent().AddObserver(this);
	m_CachedAnimator->Play("idle");
}

bombGame::PlayerComponent::~PlayerComponent()
{
	// This destructor CAN BE MADE default but I choose to leave it for a little bit of a history:

	// ------------------------------------------------------------------------------------------------

	// Old, Callback method for calling custom GAME EVENTS from health and score:

	/*if (m_CachedHealthComp)
	{
		m_CachedHealthComp->SetOnTakingDamage(nullptr);
		m_CachedHealthComp->SetOnDeath(nullptr);
	}
	if (auto* s = GetOwner()->GetComponent<ge::ScoreComponent>()) 
	{
		s->SetOnScoreChange(nullptr);
	}*/

	// ------------------------------------------------------------------------------------------------

	// Old, this WOULD work if the component cleanup order ISSUE didn't exist
	// Sadly, this is not the case

	/*m_CachedAnimator->GetOnAnimationFinishedEvent().RemoveObserver(this);
	m_CachedBoxCollider->GetOnCollisionEnterEvent().RemoveObserver(this);
	m_CachedBoxCollider->GetOnCollisionExitEvent().RemoveObserver(this);*/

	// => an IObserver registers its own subjects references and automatically unsubscribes, that's how it is now
}

void bombGame::PlayerComponent::UpdateComponent(float delatTime)
{
	UpdateAnimationLogic();
	UpdateMovementSound(delatTime);
	m_MovedThisFrame = false;
}

void bombGame::PlayerComponent::TryMove(const glm::vec3& direction, float deltaTime)
{
	// Stop movement
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

glm::vec3 bombGame::PlayerComponent::GetPlayerMidPoint() const noexcept
{
	glm::vec3 midPoint{ m_CachedOwnerTransform->GetWorldPosition() };
	if (m_CachedAnimator)
	{
		midPoint += glm::vec3{ m_CachedAnimator->GetSingleFrameRectSize().x,
			m_CachedAnimator->GetSingleFrameRectSize().y, 0.f };
	}

	return midPoint;
}

bool PlayerComponent::IsAlive() const noexcept
{
	return !m_CachedHealthComp->IsDead();
}

void bombGame::PlayerComponent::Notify(int eventId, ge::GameObject* other)
{
	if (!other) 
		return;

	switch (eventId)
	{
		// !!!
	case ge::EngineEventId::HEALTH_TAKING_DAMAGE:
		m_DamageEvent.NotifyObservers(GameEventId::PLAYER_LOST_HEALTH, GetOwner());
		/*GetOwner()->RemoveComponent<ge::BoxCollider>();*/
		break;
	case ge::EngineEventId::HEALTH_DIED:
		m_DeadEvent.NotifyObservers(GameEventId::PLAYER_DIED, GetOwner());
		m_CachedAnimator->Play("death");
		GetOwner()->RemoveComponent<ge::BoxCollider>();
		break;
	case ge::EngineEventId::SCORE_CHANGED:
		m_ScoreChangeEvent.NotifyObservers(GameEventId::PLAYER_SCORE_CHANGED, GetOwner());
		break;
		// --------
	case ge::EngineEventId::COLLISION_ENTER:
	{
		ge::Collider* otherColl{ other->GetComponent<ge::BoxCollider>() };
		if (!otherColl)
			otherColl = other->GetComponent<ge::CircleCollider>();
		if (!otherColl)
			return;

		const auto& tag{ otherColl->GetLayerTag() };

		OnCollisionEnter(other, tag);
	}
	break;
	case ge::EngineEventId::ANIMATION_FINISHED:
		// This for after death animation finished
		// do nothing for now
		/*if (!IsAlive())
		{
			GetOwner()->MarkForDeletion();
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

bool bombGame::PlayerComponent::WouldOverlapWall(const glm::vec3& worldPos) const
{
	// Build and check hyphotetical NEW collider
	const auto boxBounds{ m_CachedBoxCollider->GetBoundsAt(worldPos) };

	return ge::CollisionSystem::GetInstance().AnyOverlapWithTags(boxBounds, { "Wall", "BreakableWall" });
}

void bombGame::PlayerComponent::UpdateAnimationLogic()
{
	if (!IsAlive())
	{
		m_MovedThisFrame = false;
		return;
	}

	if (!m_CachedAnimator)
		return;

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
}

void bombGame::PlayerComponent::UpdateMovementSound(float deltaTime)
{
	if (!m_MovedThisFrame)
	{
		m_StepTimer = m_StepInterval;
		return;
	}

	m_StepTimer += deltaTime;
	if (m_StepTimer >= m_StepInterval)
	{
		m_StepTimer = 0.f;

		/*if (std::abs(m_LastMoveDir.y) > std::abs(m_LastMoveDir.x))
			m_MovedVertEvent.NotifyObservers(GameEventId::PLAYER_MOVED_VERT, GetOwner());
		else
			m_MovedHorEvent.NotifyObservers(GameEventId::PLAYER_MOVED_HOR, GetOwner());*/

		if (m_LastMoveDir.y != 0.f)
			m_MovedVertEvent.NotifyObservers(GameEventId::PLAYER_MOVED_VERT, GetOwner());
		if (m_LastMoveDir.x != 0.f)
			m_MovedHorEvent.NotifyObservers(GameEventId::PLAYER_MOVED_HOR, GetOwner());
	}
}
