#include "BombLayerComponent.h"

#include "BombComponent.h"
#include "GameEvents.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Components/PlayerComponent.h"
#include "Texture2D.h"
#include "SpawnUtils.h"
#include "LevelGrid.h"

#include <utility>
#include <optional>
#include <vector>

bombGame::BombLayerComponent::BombLayerComponent(ge::GameObject* owner, LevelGrid& levelGridRef,
	ge::SpriteSheet* bombSheetRef, std::array<ge::SpriteSheet*, 3> explosionSheetsRef,
	float startingWindup, int startingExplArm, int maxBombs)
	:Component::Component(owner),
	m_MaxBombs{ maxBombs },
	m_BombSheetRef{ bombSheetRef },
	m_ExplosionSheetsRef{ explosionSheetsRef },
	m_WindupDuration{ startingWindup },
	m_ExplosionArmLength{ startingExplArm },
	m_LevelGridRef{ levelGridRef }
{
}

bool bombGame::BombLayerComponent::TryLayBomb(const glm::vec3& position)
{
	glm::vec3 midReceivedPosition{ position }; // position parameter is a fallback

	// Check if owner has PlayerComponent and if is alive to get its mid point
	if (auto* pc = GetOwner()->GetComponent<PlayerComponent>())
	{
		midReceivedPosition = pc->GetPlayerMidPoint();
		if (!pc->IsAlive())
			return false;
	}

	const auto bombTile{ m_LevelGridRef.GetGridTileAt(midReceivedPosition) };
	if (!bombTile)
		return false;

	if (!CanLayBombAt(bombTile->col, bombTile->row))
		return false;

	const auto gridTileMidPos{ m_LevelGridRef.GetMidGridTilePointAt(midReceivedPosition) };

	// Adds BombComponent to the new bomb gameobject
	auto bomb{ spawnUtils::CreateBomb(m_LevelGridRef, {gridTileMidPos->x, gridTileMidPos->y, 0.f},
		m_BombSheetRef, m_ExplosionSheetsRef, m_WindupDuration, m_ExplosionArmLength) };
	auto* rawBombGO{ bomb.get() };
	ge::SceneManager::GetInstance().GetCurrentActiveScene()->Add(std::move(bomb));

	// Register bomb to Grid AND this Layer Component
	m_LevelGridRef.RegisterBombAt(bombTile->col, bombTile->row, rawBombGO);
	RegisterLaidBomb(rawBombGO);

	m_LaidBombEvent.NotifyObservers(events::GameEventId::LAY_BOMB, GetOwner());
	return true;
}

bool bombGame::BombLayerComponent::CanLayBombAt(int tileCol, int tileRow) const noexcept
{
	// Can't lay on top of another bomb
	if (m_LevelGridRef.GetBombAt(tileCol, tileRow))
		return false;

	if (static_cast<int>(m_ActiveBombs.size()) >= m_MaxBombs)
		return false;

	return true;
}

void bombGame::BombLayerComponent::SetMaxBombs(int newMaxBombNr) noexcept
{
	m_MaxBombs = newMaxBombNr;
}

void bombGame::BombLayerComponent::SetExplosionArmLength(int newLength)
{
	m_ExplosionArmLength = newLength;
}

void bombGame::BombLayerComponent::EnableRemoteDetonation() noexcept
{
	m_CanRemoteDetonate = true;
}

void bombGame::BombLayerComponent::DetonateAllBombsIfPossible()
{
	if (m_CanRemoteDetonate)
	{
		for (auto* bomb : m_ActiveBombs)
		{
			if (auto* bComp = bomb->GetComponent<BombComponent>())
			{
				bComp->ForceDetonate();
			}
		}
	}
}

void bombGame::BombLayerComponent::RegisterLaidBomb(ge::GameObject* bomb)
{
	if (!bomb)
		return;

	auto bombComp{ bomb->GetComponent<BombComponent>() };
	bombComp->GetExplodedBombEvent().AddObserver(this);

	m_ActiveBombs.push_back(bomb); // Add cached ref to Layer
}

void bombGame::BombLayerComponent::Notify(int eventId, ge::GameObject* sourceObj)
{
	if (static_cast<events::GameEventId>(eventId) == events::GameEventId::EXPLODED_BOMB)
	{
		std::erase(m_ActiveBombs, sourceObj); // Remove cached ref from Layer

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
