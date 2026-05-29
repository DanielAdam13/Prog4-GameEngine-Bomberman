#include "ExplosionComponent.h"
#include "GameObject.h"

bombGame::ExplosionComponent::ExplosionComponent(ge::GameObject* owner, float explosionDuration)
	:Component::Component(owner),
	m_ExplosionDuration{ explosionDuration }
{
}

void bombGame::ExplosionComponent::UpdateComponent(float deltaTime)
{
	// Simply deletes the game object after a duration, that's all this component does
	m_ExplosionTimer += deltaTime;
	if (m_ExplosionTimer >= m_ExplosionDuration)
	{
		m_ExplosionTimer = 0.f;

		GetOwner()->MarkForDeletion();
	}
}
