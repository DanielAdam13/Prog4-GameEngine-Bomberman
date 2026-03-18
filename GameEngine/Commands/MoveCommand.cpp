#include "MoveCommand.h"
#include "GameObject.h"
#include "Components/Transform.h"

using namespace ge;

MoveCommand::MoveCommand(GameObject* commandTarget, const glm::vec3& startingDirection, float moveSpeed)
	:GameObjectCommand::GameObjectCommand(commandTarget),
	m_pTargetTransform{ GetCommandTarget()->GetComponent<Transform>() },
	m_ConstantMoveDirection{ startingDirection },
	m_MoveSpeed{ moveSpeed }
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
	m_pTargetTransform->SetLocalPosition(m_pTargetTransform->GetLocalPosition() + 
		m_ConstantMoveDirection * m_MoveSpeed * deltaTime);
}
