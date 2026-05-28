#include "CameraFollowComponent.h"
#include "GameObject.h"
#include "Camera.h"

#include <algorithm>

ge::CameraFollowComponent::CameraFollowComponent(GameObject* ownerPtr, Camera* camera, float smoothFactor)
	:Component::Component(ownerPtr),
	m_Camera{ camera },
	m_SmoothFactor{ smoothFactor }
{
}

void ge::CameraFollowComponent::AddTarget(GameObject* newTarget)
{
	m_Targets.push_back(newTarget);
}

void ge::CameraFollowComponent::UpdateComponent(float deltaTime)
{
	glm::vec2 targetPos{ ComputeAverageTargetPosition() };

	// Lerp
	const auto current{ m_Camera->GetPosition() };
	const auto newPos{ current + (targetPos - current) * std::min(m_SmoothFactor * deltaTime, 1.f) };
	
	m_Camera->SetPosition(newPos);
}

glm::vec2 ge::CameraFollowComponent::ComputeAverageTargetPosition() const noexcept
{
	glm::vec2 sum{ 0, 0 };
	int count{ 0 };
	for (auto* t : m_Targets)
	{
		if (t && !t->MarkedForDeletion())
		{
			auto pos{ t->GetComponent<Transform>()->GetWorldPosition() };
			sum += glm::vec2{ pos.x, pos.y };
			++count;
		}
	}

	if (count > 0)
	{
		return sum / static_cast<float>(count);
	}

	return sum;
}
