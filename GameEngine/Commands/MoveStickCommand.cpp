#include "MoveStickCommand.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Components/Transform.h"

using namespace ge;

MoveStickCommand::MoveStickCommand(GameObject* target, float speed)
	:GameObjectCommand::GameObjectCommand(target),
	m_pTargetTransform{ GetCommandTarget()->GetComponent<Transform>() },
	m_MoveSpeed{ speed }
{
}

void MoveStickCommand::Execute(float deltaTime)
{
	MoveController(deltaTime);
}

void MoveStickCommand::MoveController(float deltaTime)
{
	const glm::vec2 stick{ InputManager::GetInstance().GetLeftStick() };

	// Guard against deadzone, even after it was done in GetLeftStick
	if (glm::length(stick) < 0.01f) 
		return;

	const glm::vec3 direction{ stick.x, -stick.y, 0.f };

	m_pTargetTransform->SetLocalPosition(m_pTargetTransform->GetLocalPosition() +
		direction * m_MoveSpeed * deltaTime);
}
