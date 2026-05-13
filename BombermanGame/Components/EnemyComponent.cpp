#include "EnemyComponent.h"

#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/Transform.h"
#include "EnemyStates/ChaseState.h"
#include "EnemyStates/WanderState.h"

bombGame::EnemyComponent::EnemyComponent(ge::GameObject* owner, float speed, float detectionRadius)
	:Component::Component(owner),
	m_OwnerTransformRef{ owner->GetComponent<ge::Transform>() },
	m_Speed{speed},
	m_DetectionRadius{ detectionRadius },
	m_ChaseState{ nullptr },
	m_WanderState{ nullptr },
	m_CurrentState{ nullptr }
{
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
	auto* healthComp{ GetOwner()->GetComponent<ge::HealthComponent>() };
	return healthComp && !healthComp->IsDead();
}
