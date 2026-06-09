#include "CameraPlayerFollowComponent.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Camera.h"
#include "Components/PlayerComponent.h"

bombGame::CameraPlayerFollowComponent::CameraPlayerFollowComponent(ge::GameObject* ownerPtr, ge::Camera* camera, float smoothFactor)
	:Component::Component(ownerPtr),
	m_Camera{ camera },
	m_SmoothFactor{ smoothFactor }
{
}

void bombGame::CameraPlayerFollowComponent::AddTarget(ge::GameObject* newTarget)
{
	m_Targets.push_back(newTarget);
}

void bombGame::CameraPlayerFollowComponent::UpdateComponent(float deltaTime)
{
	glm::vec2 targetPos{ ComputeAverageTargetPosition() };

	// Lerp
	const auto current{ m_Camera->GetPosition() };
	const auto newPos{ current + (targetPos - current) * std::min(m_SmoothFactor * deltaTime, 1.f) };

	m_Camera->SetPosition(newPos);
}

glm::vec2 bombGame::CameraPlayerFollowComponent::ComputeAverageTargetPosition() const noexcept
{
	glm::vec2 sum{ 0, 0 };
	int count{ 0 };
	for (auto* playerTarget : m_Targets)
	{
		// GameObject Valid
		if (!playerTarget || playerTarget->MarkedForDeletion())
			continue;

		// Target Has PlayerComponent
		auto* plComp{ playerTarget->GetComponent<PlayerComponent>() };
		if (!plComp)
			continue;
		
		// Player is Alive
		if (!plComp->IsAlive())
			continue;

		auto pos{ playerTarget->GetComponent<ge::Transform>()->GetWorldPosition() };
		sum += glm::vec2{ pos.x, pos.y };
		++count;
	}

	if (count > 0)
	{
		return sum / static_cast<float>(count);
	}

	return sum;
}
