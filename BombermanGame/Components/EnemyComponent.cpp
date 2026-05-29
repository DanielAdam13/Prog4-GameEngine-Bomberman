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

bombGame::EnemyComponent::EnemyComponent(ge::GameObject* owner, float speed, float detectionRadius)
	:Component::Component(owner),
	m_OwnerTransformRef{ owner->GetComponent<ge::Transform>() },
	m_Speed{speed},
	m_DetectionRadius{ detectionRadius },
	m_ChaseState{ nullptr },
	m_WanderState{ nullptr },
	m_CurrentState{ nullptr },
	m_CachedHealthComp{ owner->GetComponent<ge::HealthComponent>() },
	m_CachedBoxCollider{ owner->GetComponent<ge::BoxCollider>() }
{
	assert(m_CachedHealthComp && "Enemy Component requires a HealthComponent on the same GameObject");

	assert(m_CachedBoxCollider && "Enemy Component requires a Box Collider on the same GameObject");
 	m_CachedBoxCollider->GetOnCollisionEnterEvent().AddObserver(this);
}

void bombGame::EnemyComponent::UpdateComponent(float deltaTime)
{
	if (!m_CurrentState)
		return;

	m_CurrentState->OnUpdate(deltaTime);
}

void bombGame::EnemyComponent::InitializeStates()
{
	m_ChaseState = std::make_unique<ChaseState>(GetOwner());
	m_WanderState = std::make_unique<WanderState>(GetOwner());
	m_CurrentState = m_ChaseState.get();
}

void bombGame::EnemyComponent::TransitionToWander()
{
	if (m_CurrentState)
		m_CurrentState->OnExit();

	m_CurrentState = m_WanderState.get();

	m_CurrentState->OnEnter();
}

void bombGame::EnemyComponent::TransitionToChase()
{
	if (m_CurrentState)
		m_CurrentState->OnExit();

	m_CurrentState = m_ChaseState.get();

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
