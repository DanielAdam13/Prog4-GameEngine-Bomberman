#include "RotatorComponent.h"
#include "Transform.h"
#include "GameObject.h"

using namespace ge;

Rotator::Rotator(GameObject* pOwnerPtr, bool isClockwise, float radius, float angularSpeedDegrees)
	:Component::Component(pOwnerPtr),
	pOwnerTransform{ GetOwner()->GetComponent<Transform>() },
	m_ClockwiseRotation{ isClockwise },
	m_CircleCenter{ pOwnerTransform->GetWorldPosition() },
	m_Radius{ radius },
	m_AngularSpeed{ glm::radians(angularSpeedDegrees) },
	m_Angle{ 0.f }
{
}

void Rotator::UpdateComponent(float deltaTime)
{
	m_Angle += (m_ClockwiseRotation ? -1.f : 1.f) * m_AngularSpeed * deltaTime;

	const glm::vec3 newPos{ m_CircleCenter.x + m_Radius * cos(m_Angle),
	m_CircleCenter.y + m_Radius * sin(m_Angle),
	m_CircleCenter.z};

	if (pOwnerTransform->GetOwner()->GetParent())
	{
		// World needs to be Local if parent exists
		auto parentWorldMatrix{ pOwnerTransform->GetParentTransform()->GetWorldMatrix() };
		
		const glm::mat4 parentWorldInv{ glm::inverse(parentWorldMatrix) }; // For that we need the inverse of the parent's world
		const glm::vec3 localPos{ glm::vec3(parentWorldInv * glm::vec4(newPos, 1.f)) };

		pOwnerTransform->SetLocalPosition(localPos);
	}
	else
	{
		pOwnerTransform->SetLocalPosition(newPos);
	}
	
}
