#include "BombLayerComponent.h"

#include "BombComponent.h"
#include "GameEvents.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Components/PlayerComponent.h"
#include "Texture2D.h"
#include "Utils.h"
#include "LevelBuilder.h"

#include <utility>
#include <optional>

bombGame::BombLayerComponent::BombLayerComponent(ge::GameObject* owner, LevelGrid* levelGridRef,
	ge::SpriteSheet* bombSheetRef, ge::SpriteSheet* explosionSheetRef,
	std::function<float()> windupDurationFn, int maxBombs)
	:Component::Component(owner),
	m_MaxBombs{ maxBombs },
	m_BombSheetRef{ bombSheetRef },
	m_ExplosionSheetRef{ explosionSheetRef },
	m_WindupDurationFn{ std::move(windupDurationFn) },
	m_LevelGridRef{ levelGridRef }
{
}

bool bombGame::BombLayerComponent::TryLayBomb(const glm::vec3& position)
{
	// Optional: Check if owner has PlayerComponent and if is alive
	if (auto* pc = GetOwner()->GetComponent<PlayerComponent>())
	{
		if (!pc->IsAlive())
			return false;
	}

	if (!CanLayBomb())
		return false;

	const auto gridTileMidPos{ m_LevelGridRef->GetMiddlePointGridAt(position) };

	// Adds BombComponent to the new bomb gameobject
	auto bomb{ spawnUtils::CreateBomb({gridTileMidPos->x, gridTileMidPos->y, 0.f},
		m_BombSheetRef, m_ExplosionSheetRef, m_WindupDurationFn()) };
	auto* rawBombGO{ bomb.get() };
	ge::SceneManager::GetInstance().GetCurrentActiveScene()->Add(std::move(bomb));

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

void bombGame::BombLayerComponent::Notify(int eventId, ge::GameObject* sourceObj)
{
	if (static_cast<GameEventId>(eventId) == GameEventId::EXPLODED_BOMB)
	{
		if (m_ActiveBombs > 0)
			--m_ActiveBombs;

		// Per Component notify, not per bomb
		m_BombExplodedEvent.NotifyObservers(eventId, sourceObj);
	}
}

ge::Subject& bombGame::BombLayerComponent::GetLaidBombEvent() noexcept
{
	return m_LaidBombEvent;
}

ge::Subject& bombGame::BombLayerComponent::GetBombExplodedEvent() noexcept
{
	return m_BombExplodedEvent;
}
