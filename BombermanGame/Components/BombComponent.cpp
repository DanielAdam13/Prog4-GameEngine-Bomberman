#include "BombComponent.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "GameEvents.h"
#include "Utils.h"
#include "SceneManager.h"

#include <utility>

bombGame::BombComponent::BombComponent(ge::GameObject* owner, float windupDuration, ge::Texture2D* explosionTextureRef)
	:Component::Component{ owner },
	m_WindUpDuration{ windupDuration },
	m_WindupTimer{ 0.f },
	m_ExplosionTetxureRef{ explosionTextureRef }
{
}

void bombGame::BombComponent::UpdateComponent(float deltaTime)
{
	m_WindupTimer += deltaTime;
	if (m_WindupTimer >= m_WindUpDuration)
	{
		m_WindupTimer = 0.f;
		m_ExplodedBombEvent.NotifyObservers(GameEventId::EXPLODED_BOMB, GetOwner());

		auto explosionGO{ spawnUtils::CreateExplosion(
			GetOwner()->GetComponent<ge::Transform>()->GetWorldPosition(), m_ExplosionTetxureRef, 1.5f) };
		ge::SceneManager::GetInstance().GetCurrentActiveScene()->Add(std::move(explosionGO));

		//this->MarkForDeletion();
		GetOwner()->MarkForDeletion();
	}
}

void bombGame::BombComponent::RenderComponent() const
{
}
