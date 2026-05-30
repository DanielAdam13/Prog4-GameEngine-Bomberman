#include "EnemyComponent.h"

#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/Transform.h"
#include "EnemyStates/ChaseState.h"
#include "EnemyStates/WanderState.h"

#include "Components/Colliders.h"
#include "EngineEvents.h"
#include "GameEvents.h"

#include <cassert>
#include <utility>
#include <memory>

bombGame::EnemyComponent::EnemyComponent(ge::GameObject* owner, float speed, float detectionRadius)
	:Component::Component(owner),
	m_OwnerTransformRef{ owner->GetComponent<ge::Transform>() },
	m_CachedHealthComp{ owner->GetComponent<ge::HealthComponent>() },
	m_CachedBoxCollider{ owner->GetComponent<ge::BoxCollider>() },
	m_Speed{ speed },
	m_DetectionRadius{ detectionRadius }
{
	assert(m_CachedHealthComp && "Enemy Component requires a HealthComponent on the same GameObject");
	m_CachedHealthComp->GetOnTakingDamageEvent().AddObserver(this);
	m_CachedHealthComp->GetOnDeathEvent().AddObserver(this);

	assert(m_CachedBoxCollider && "Enemy Component requires a Box Collider on the same GameObject");
 	m_CachedBoxCollider->GetOnCollisionEnterEvent().AddObserver(this);
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

	// Apply movement
	const auto pos{ m_OwnerTransformRef->GetWorldPosition() };
	m_OwnerTransformRef->SetLocalPosition(pos + m_CurrentMoveDirection * m_Speed * deltaTime);
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
	}
}

ge::Subject& bombGame::EnemyComponent::GetDeadEvent() noexcept
{
	return m_DeadEvent;
}

void bombGame::EnemyComponent::OnCollisionEnter(ge::GameObject*, const ge::CollisionLayerTag& tag)
{
	if (tag == "Explosion")
	{
		if(m_CachedHealthComp)
			m_CachedHealthComp->Die();
	}
}
