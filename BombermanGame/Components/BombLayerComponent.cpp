#include "BombLayerComponent.h"

#include "BombComponent.h"
#include "GameEvents.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Utils.h"

#include <utility>

bombGame::BombLayerComponent::BombLayerComponent(ge::GameObject* owner, 
	ge::Texture2D* textureRef, std::function<float()> explosionTimerFn, int maxBombs)
	:Component::Component(owner),
	m_MaxBombs{ maxBombs },
	m_TextureBombRef{ textureRef },
	m_ExplosionTimerFn{ std::move(explosionTimerFn) }
{
}

bool bombGame::BombLayerComponent::TryLayBomb(const glm::vec3& position)
{
	if (!CanLayBomb())
		return false;

	// Adds BombComponent to the new bomb gameobject
	auto bomb{ CreateBomb(position, m_TextureBombRef, m_ExplosionTimerFn()) };
	auto* rawBombGO{ bomb.get() };
	ge::SceneManager::GetInstance().GetCurrentScene()->Add(std::move(bomb));

	RegisterLaidBomb(rawBombGO);

	m_LaidBombEvent.NotifyObservers(GameEventId::LAY_BOMB, GetOwner());
	return true;
}

bool bombGame::BombLayerComponent::CanLayBomb() const noexcept
{
	return m_ActiveBombs < m_MaxBombs;
}

int bombGame::BombLayerComponent::GetActiveBombs() const noexcept
{
	return m_ActiveBombs;
}

int bombGame::BombLayerComponent::GetMaxBombs() const noexcept
{
	return m_MaxBombs;
}

void bombGame::BombLayerComponent::SetMaxBombs(int newMaxBombNr) noexcept
{
	m_MaxBombs = newMaxBombNr;
}

void bombGame::BombLayerComponent::RegisterLaidBomb(ge::GameObject* bomb)
{
	if (!bomb)
		return;

	auto bombComp{ bomb->GetComponent<BombComponent>() };

	bombComp->GetExplodedBombEvent().AddObserver(this);
	++m_ActiveBombs;
}

void bombGame::BombLayerComponent::Notify(int eventId, ge::GameObject*)
{
	if (static_cast<GameEventId>(eventId) == GameEventId::EXPLODED_BOMB)
	{
		if (m_ActiveBombs > 0)
			--m_ActiveBombs;
	}
}

ge::Subject& bombGame::BombLayerComponent::GetLaidBombEvent() noexcept
{
	return m_LaidBombEvent;
}
