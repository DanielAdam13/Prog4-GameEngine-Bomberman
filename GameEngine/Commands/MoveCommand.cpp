#include "MoveCommand.h"
#include "GameObject.h"
#include "Components/Transform.h"

using namespace ge;

MoveCommand::MoveCommand(GameObject* target)
	:GameObjectCommand::GameObjectCommand(target),
	m_pTargetTransform{ GetCommandTarget()->GetComponent<Transform>()}
{
}

void MoveCommand::Execute(float deltaTime)
{
	Move(deltaTime);
}

void MoveCommand::Move(float deltaTime)
{
	m_pTargetTransform->SetLocalPosition(m_pTargetTransform->GetLocalPosition() + 10.f * deltaTime);
}
