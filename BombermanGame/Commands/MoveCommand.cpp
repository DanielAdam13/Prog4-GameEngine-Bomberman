#include "MoveCommand.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/PlayerComponent.h"

#include <utility>

bombGame::MoveCommand::MoveCommand(ge::GameObject* commandTarget, const glm::vec3& startingDirection, float moveSpeed)
	:ge::GameObjectCommand::GameObjectCommand(commandTarget),
	m_pTargetTransform{ GetCommandTarget()->GetComponent<ge::Transform>() },
	m_ConstantMoveDirection{ startingDirection },
	m_MoveSpeed{ moveSpeed }
{
}

bombGame::MoveCommand::MoveCommand(ge::GameObject* commandTarget, const glm::vec3& startingDirection, std::function<float()> speedFunc)
	:GameObjectCommand::GameObjectCommand(commandTarget),
	m_pTargetTransform{ GetCommandTarget()->GetComponent<ge::Transform>() },
	m_ConstantMoveDirection{ startingDirection },
	m_MoveSpeed{ speedFunc() },
	m_SpeedFunc{ std::move(speedFunc) }
{
}

void bombGame::MoveCommand::Execute(float deltaTime)
{
	Move(deltaTime);
}

void bombGame::MoveCommand::SetSpeed(float newSpeed)
{
	m_MoveSpeed = newSpeed;
}

float bombGame::MoveCommand::GetSpeed() const noexcept
{
	return m_MoveSpeed;
}

void bombGame::MoveCommand::Move(float deltaTime)
{
	if (auto* pc = GetCommandTarget()->GetComponent<PlayerComponent>())
	{
		pc->TryMove(m_ConstantMoveDirection, deltaTime);
		return;
	}

	// Fallback so Move is not specific only to PlayerComponent
	if (m_SpeedFunc)
	{
		m_MoveSpeed = m_SpeedFunc();
	}
	m_pTargetTransform->SetLocalPosition(m_pTargetTransform->GetLocalPosition() + 
		m_ConstantMoveDirection * m_MoveSpeed * deltaTime);
}
