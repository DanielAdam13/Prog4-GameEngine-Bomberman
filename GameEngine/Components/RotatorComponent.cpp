#include "RotatorComponent.h"
#include "Transform.h"
#include "GameObject.h"

using namespace ge;

Rotator::Rotator(GameObject* pOwnerPtr, float angularSpeedDegrees, float radius )
	:Component::Component(pOwnerPtr),
	pOwnerTransform{ GetOwner()->GetComponent<Transform>() },
	m_CircleCenter{ pOwnerTransform->GetWorldPosition() },
	m_Radius{ radius },
	m_AngularSpeed{ glm::radians(angularSpeedDegrees) },
	m_Angle{ 0.f }
{
}

void Rotator::UpdateComponent(float deltaTime)
{
	m_Angle += m_AngularSpeed * deltaTime;

	// Overflow of angle
	if (std::abs(m_Angle) > glm::radians(360.f))
		m_Angle = 0.f;

	const glm::vec3 newPos{ m_CircleCenter.x + m_Radius * cos(m_Angle),
	m_CircleCenter.y + m_Radius * sin(m_Angle),
	m_CircleCenter.z};


	// TODO: CHANGEEEEE...
	if (pOwnerTransform->GetOwner()->GetParent())
	{
		// World to Local if parent exists
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
