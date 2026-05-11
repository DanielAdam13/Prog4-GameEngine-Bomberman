#include "MoveCommand.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include <utility>

using namespace ge;

MoveCommand::MoveCommand(GameObject* commandTarget, const glm::vec3& startingDirection, float moveSpeed)
	:GameObjectCommand::GameObjectCommand(commandTarget),
	m_pTargetTransform{ GetCommandTarget()->GetComponent<Transform>() },
	m_ConstantMoveDirection{ startingDirection },
	m_MoveSpeed{ moveSpeed }
{
}

ge::MoveCommand::MoveCommand(GameObject* commandTarget, const glm::vec3& startingDirection, std::function<float()> speedFunc)
	:GameObjectCommand::GameObjectCommand(commandTarget),
	m_pTargetTransform{ GetCommandTarget()->GetComponent<Transform>() },
	m_ConstantMoveDirection{ startingDirection },
	m_MoveSpeed{ speedFunc() },
	m_SpeedFunc{ std::move(speedFunc) }
{
}

void MoveCommand::Execute(float deltaTime)
{
	Move(deltaTime);
}

void MoveCommand::SetSpeed(float newSpeed)
{
	m_MoveSpeed = newSpeed;
}

float MoveCommand::GetSpeed() const noexcept
{
	return m_MoveSpeed;
}

void MoveCommand::Move(float deltaTime)
{
	if (m_SpeedFunc)
	{
		m_MoveSpeed = m_SpeedFunc();
	}
	m_pTargetTransform->SetLocalPosition(m_pTargetTransform->GetLocalPosition() + 
		m_ConstantMoveDirection * m_MoveSpeed * deltaTime);
}
