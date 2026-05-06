#include "WanderState.h"

#include "GameObject.h"
#include "Components/EnemyComponent.h"
#include "Components/Transform.h"

#include <random>

bombGame::WanderState::WanderState(ge::GameObject* pSourcePtr)
	:ge::State::State(pSourcePtr),
	m_CurrentWanderDirection{ m_PossibleDirections[0] },
	m_WanderTimer{ 0.f },
	m_pSourceTransform{ pSourcePtr->GetComponent<ge::Transform>() },
	m_pEnemyController{ pSourcePtr->GetComponent<EnemyComponent>() }
{
	// Set at start
	m_pEnemyController->SetMoveDirection(m_CurrentWanderDirection);
}

void bombGame::WanderState::OnUpdate(float deltaTime)
{
	// Switch direction logic:
	m_WanderTimer += deltaTime;

	if (m_WanderTimer >= DirectionSwitchTimer)
	{
		static std::mt19937 gen{ std::random_device{}() };
		static std::uniform_int_distribution<size_t> dist(0, m_PossibleDirections.size() - 1);

		m_CurrentWanderDirection = m_PossibleDirections[dist(gen)];
		m_pEnemyController->SetMoveDirection(m_CurrentWanderDirection);

		m_WanderTimer -= DirectionSwitchTimer;
	}

	// Actual Movement:
	glm::vec3 pos{ m_pSourceTransform->GetWorldPosition() };
	pos += m_CurrentWanderDirection * m_pEnemyController->GetSpeed() * deltaTime;
	m_pSourceTransform->SetLocalPosition({ pos.x, pos.y, 0.f });
}
