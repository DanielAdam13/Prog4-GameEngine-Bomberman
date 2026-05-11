#include "BombComponent.h"
#include "GameObject.h"

#include "GameEvents.h"

bombGame::BombComponent::BombComponent(ge::GameObject* owner, float explosionTimer)
	:Component::Component{ owner },
	m_WindUpDuration{ explosionTimer },
	m_ExplosionTimer{ 0.f }
{
}

void bombGame::BombComponent::UpdateComponent(float deltaTime)
{
	m_ExplosionTimer += deltaTime;
	if (m_ExplosionTimer >= m_WindUpDuration)
	{
		m_ExplosionTimer = 0.f;
		m_ExplodedBombEvent.NotifyObservers(GameEventId::EXPLODED_BOMB, GetOwner());

		//this->MarkForDeletion();
		GetOwner()->MarkForDeletion();
	}
}

void bombGame::BombComponent::RenderComponent() const
{
}
