#include "WanderState.h"

#include "GameObject.h"
#include "Components/EnemyComponent.h"
#include "Components/Transform.h"

#include <random>

bombGame::WanderState::WanderState(ge::GameObject* pTargetPtr)
	:ge::State::State(pTargetPtr),
	m_CurrentWanderDirection{ m_PossibleDirections[0] },
	m_WanderTimer{ 0.f }
{
	// Set at start
	GetSource()->GetComponent<EnemyComponent>()->SetMoveDirection(m_CurrentWanderDirection);
}

void bombGame::WanderState::OnUpdate(float deltaTime)
{
	auto* enemyController{ GetSource()->GetComponent<EnemyComponent>() };

	// Switch direction:
	m_WanderTimer += deltaTime;

	if (m_WanderTimer >= DirectionSwitchTimer)
	{
		static std::mt19937 gen{ std::random_device{}() };
		static std::uniform_int_distribution<size_t> dist(0, m_PossibleDirections.size() - 1);

		m_CurrentWanderDirection = m_PossibleDirections[dist(gen)];
		enemyController->SetMoveDirection(m_CurrentWanderDirection);

		m_WanderTimer -= DirectionSwitchTimer;
	}

	// Actual Movement:
	auto* targetTransform{ GetSource()->GetComponent<ge::Transform>() };

	glm::vec3 pos{ targetTransform->GetWorldPosition() };

	pos += m_CurrentWanderDirection * enemyController->GetSpeed() * deltaTime;

	targetTransform->SetLocalPosition({ pos.x, pos.y, 0.f });

}
