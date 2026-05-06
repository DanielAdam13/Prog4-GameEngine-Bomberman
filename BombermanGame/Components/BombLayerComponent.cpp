#include "BombLayerComponent.h"

#include "BombComponent.h"
#include "GameEvents.h"
#include "GameObject.h"

bombGame::BombLayerComponent::BombLayerComponent(ge::GameObject* owner, int maxBombs)
	:Component::Component(owner),
	m_MaxBombs{ maxBombs }
{
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
	if (!bombComp)
		return;

	bombComp->GetExplodedBombEvent().AddObserver(this);
	++m_ActiveBombs;
}

void bombGame::BombLayerComponent::Notify(int eventId, ge::GameObject*)
{
	if (static_cast<EventId>(eventId) == EventId::EXPLODED_BOMB)
	{
		if (m_ActiveBombs > 0)
			--m_ActiveBombs;
	}
}
