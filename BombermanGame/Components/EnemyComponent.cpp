#include "EnemyComponent.h"

#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/Transform.h"
#include "EnemyStates/ChaseState.h"
#include "EnemyStates/WanderState.h"
#include "LevelBuilder.h"

#include "Components/Colliders.h"
#include "EngineEvents.h"
#include "GameEvents.h"

#include <cassert>
#include <utility>
#include <memory>

bombGame::EnemyComponent::EnemyComponent(ge::GameObject* owner, LevelGrid* levelGrid, float speed, float detectionRadius)
	:Component::Component(owner),
	m_OwnerTransformRef{ owner->GetComponent<ge::Transform>() },
	m_CachedHealthComp{ owner->GetComponent<ge::HealthComponent>() },
	m_CachedBoxCollider{ owner->GetComponent<ge::BoxCollider>() },
	m_CachedAnimator{ owner->GetComponent<ge::AnimatorComponent>() },
	m_LevelGrid{ levelGrid },
	m_Speed{ speed },
	m_DetectionRadius{ detectionRadius }
{
	assert(m_CachedHealthComp && "EnemyComponent requires a HealthComponent on the same GameObject");
	m_CachedHealthComp->GetOnTakingDamageEvent().AddObserver(this);
	m_CachedHealthComp->GetOnDeathEvent().AddObserver(this);

	assert(m_CachedBoxCollider && "EnemyComponent requires a Box Collider on the same GameObject");
 	m_CachedBoxCollider->GetOnCollisionEnterEvent().AddObserver(this);

	assert(m_CachedAnimator && "EnemyComponent requires an Animator Component on the same GameObject");
	m_CachedAnimator->GetOnAnimationFinishedEvent().AddObserver(this);
	m_CachedAnimator->Play("idle");
}

void bombGame::EnemyComponent::UpdateComponent(float deltaTime)
{
	// 1. Apply ACTUAL State switch - pending transition
	if (m_TransitionPending) 
	{
		if (m_CurrentState) 
			m_CurrentState->OnExit();

		m_CurrentState = std::move(m_PendingState);
		m_TransitionPending = false;

		if (m_CurrentState) 
			m_CurrentState->OnEnter();
	}

	if (!m_CurrentState)
		return;

	// 2. Tick current state.
	// If next does not return nullptr (nullptr means no change):
	if (auto next = m_CurrentState->OnUpdate(deltaTime))
	{
		m_PendingState = std::move(next);
		m_TransitionPending = true;
	}

	if (!IsAlive())
		return;

	// ===================================
	// Apply movement
	// ===================================
	
	// 3. Pick new Tile to move to
	if (!m_HasTileTarget)
	{
		PickNextTileTarget();
	}

	// 4. Move toward the tile target
	const auto pos{ m_OwnerTransformRef->GetWorldPosition() };
	m_OwnerTransformRef->SetLocalPosition(pos + m_CurrentMoveDirection * m_Speed * deltaTime);

	// 5. Check arrival
	if (m_HasTileTarget && HasArrivedAtTargetTile())
	{
		// Snap position to tile center
		GetOwnerTransform()->SetLocalPosition({ m_TargetTileCenter->x, m_TargetTileCenter->y, 0.f });
		m_HasTileTarget = false;
	}

	// 6. After everything, animate
	UpdateAnimationLogic();
}

void bombGame::EnemyComponent::InitializeStates()
{
	m_CurrentState = std::make_unique<WanderState>(GetOwner());
	if (m_CurrentState)
		m_CurrentState->OnEnter();
}

void bombGame::EnemyComponent::AddTarget(ge::GameObject* newTarget) noexcept
{
	if (!newTarget)
		return;

	auto* targetTr{ newTarget->GetComponent<ge::Transform>() };

	m_Targets.push_back(newTarget);
	m_TargetTransforms.push_back(targetTr);
}

bool bombGame::EnemyComponent::IsAlive() const noexcept
{
	return m_CachedHealthComp && !m_CachedHealthComp->IsDead();
}

void bombGame::EnemyComponent::Notify(int eventId, ge::GameObject* other)
{
	switch (eventId)
	{
	case ge::EngineEventId::HEALTH_DIED:
		m_DeadEvent.NotifyObservers(GameEventId::ENEMY_DIED, GetOwner());
		m_CachedAnimator->Play("death");
		GetOwner()->RemoveComponent<ge::BoxCollider>();
		break;
	case ge::EngineEventId::COLLISION_ENTER:
	{
		ge::Collider* otherColl{ other->GetComponent<ge::BoxCollider>() };
		if (!otherColl)
			otherColl = other->GetComponent<ge::CircleCollider>();
		if (!otherColl)
			return;

		const auto& tag{ otherColl->GetLayerTag() };

		OnCollisionEnter(other, tag);
		break;
	}
	case ge::EngineEventId::ANIMATION_FINISHED:
		if (!IsAlive())
		{
			GetOwner()->MarkForDeletion();
		}
		break;
	}
}

ge::Subject& bombGame::EnemyComponent::GetDeadEvent() noexcept
{
	return m_DeadEvent;
}

void bombGame::EnemyComponent::PickNextTileTarget()
{
	const auto currentTile{ m_LevelGrid->GetGridTileAt(GetOwnerTransform()->GetWorldPosition()) };
	if (!currentTile) // If invalid tile, early out
		return;

	const glm::vec3 dir{ m_CurrentState->ChooseDirectionAtIntersection(*currentTile) };

	// If state wants us to stop, early out
	if (dir == glm::vec3{ 0, 0, 0 })
	{
		m_CurrentMoveDirection = { 0, 0, 0 };
		m_HasTileTarget = false;
		return;
	}

	const int nextCol{ currentTile->col + static_cast<int>(dir.x) };
	const int nextRow{ currentTile->row + static_cast<int>(dir.y) };
	auto nextTile{ m_LevelGrid->GetGridTileByCoord(nextCol, nextRow) };

	// Just in case check. States should not return blocked directions but still:
	if (!nextTile || m_LevelGrid->IsBlocked(*nextTile))
	{
		m_CurrentMoveDirection = { 0, 0, 0 };
		m_HasTileTarget = false;
		return;
	}

	m_TargetTileCenter = m_LevelGrid->GetMidGridTilePointByCoord(nextCol, nextRow);
	m_CurrentMoveDirection = dir;
	m_HasTileTarget = true;
}

bool bombGame::EnemyComponent::HasArrivedAtTargetTile() const
{
	const auto pos{ GetOwnerTransform()->GetWorldPosition() };
	const auto diff{ glm::vec2{*m_TargetTileCenter} - glm::vec2{pos.x, pos.y} };

	static constexpr float arrivalEpsilon{ 2.f };

	return glm::length(diff) < arrivalEpsilon;
}

void bombGame::EnemyComponent::OnCollisionEnter(ge::GameObject*, const ge::CollisionLayerTag& tag)
{
	if (tag == "Explosion")
	{
		if(m_CachedHealthComp)
			m_CachedHealthComp->Die();
	}
}

void bombGame::EnemyComponent::UpdateAnimationLogic()
{
	if (!m_CachedAnimator)
		return;

	if (m_CurrentMoveDirection.y < -0.5f)
		m_CachedAnimator->Play("walk_left");
	else if (m_CurrentMoveDirection.y > 0.5f)
		m_CachedAnimator->Play("walk_right");
	else if (m_CurrentMoveDirection.x < -0.5f)
		m_CachedAnimator->Play("walk_left");
	else if (m_CurrentMoveDirection.x > 0.5f)
		m_CachedAnimator->Play("walk_right");
	else
		m_CachedAnimator->Play("idle");
}
