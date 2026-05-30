#include "WanderState.h"

#include "GameObject.h"
#include "Components/EnemyComponent.h"
#include "Components/Transform.h"
#include "ChaseState.h"

#include <random>

bombGame::WanderState::WanderState(ge::GameObject* pSourcePtr)
	:EnemyState::EnemyState(pSourcePtr),
	m_CurrentWanderDirection{ m_PossibleDirections[0] }
{
	// Set at start
	GetSourceEnemyComponent()->SetMoveDirection(m_CurrentWanderDirection);
}

void bombGame::WanderState::OnEnter()
{
	m_WanderTimer = 0.f;
	m_PlayerCheckTimer = 0.f;
	m_CurrentWanderDirection = m_PossibleDirections[1];
	GetSourceEnemyComponent()->SetMoveDirection(m_CurrentWanderDirection);
}

void bombGame::WanderState::OnExit()
{
	m_CurrentWanderDirection = {};
}

std::unique_ptr<bombGame::EnemyState> bombGame::WanderState::OnUpdate(float deltaTime)
{
	// 1. Is there a close target check
	m_PlayerCheckTimer += deltaTime;
	if (m_PlayerCheckTimer >= m_PlayerCheckInterval)
	{
		m_PlayerCheckTimer = 0.f;

		// IF CLOSEST PLAYER TARGET is not nullptr, if it exists
		if (FindClosestPlayerInRange()) 
		{
			// Transition to chase
			return std::make_unique<ChaseState>(GetSourceEnemyComponent()->GetOwner());
		}
	}

	// 2. Switch direction logic:
	m_WanderTimer += deltaTime;
	if (m_WanderTimer >= DirectionSwitchTimer)
	{
		m_WanderTimer -= DirectionSwitchTimer;

		static std::mt19937 gen{ std::random_device{}() };
		static std::uniform_int_distribution<size_t> dist(0, m_PossibleDirections.size() - 1);

		m_CurrentWanderDirection = m_PossibleDirections[dist(gen)];
		GetSourceEnemyComponent()->SetMoveDirection(m_CurrentWanderDirection);
	}

	// 3. Stay in current state
	return nullptr;
}
