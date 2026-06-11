#include "BombComponent.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "GameEvents.h"
#include "SpawnUtils.h"
#include "SceneManager.h"
#include "LevelGrid.h"

bombGame::BombComponent::BombComponent(ge::GameObject* owner, LevelGrid& grid, 
	float windupDuration, int explosionArmLength, std::array<ge::SpriteSheet*, 3>& explosionSheetRef)
	:Component::Component{ owner },
	m_WindUpDuration{ windupDuration },
	m_WindupTimer{ 0.f },
	m_ExplosionSheetsRef{ explosionSheetRef },
	m_CachedGrid{ grid },
	m_ExplosionArmLength{ explosionArmLength }
{
}

void bombGame::BombComponent::UpdateComponent(float deltaTime)
{
	m_WindupTimer += deltaTime;
	if (m_WindupTimer >= m_WindUpDuration)
	{
		m_WindupTimer = 0.f;

		// Clear from Grid before despawning
		const auto bombTile{ m_CachedGrid.GetGridTileAt(
			GetOwner()->GetComponent<ge::Transform>()->GetWorldPosition()) };
		if (bombTile)
			m_CachedGrid.ClearBombAt(bombTile->col, bombTile->row);

		m_ExplodedBombEvent.NotifyObservers(GameEventId::EXPLODED_BOMB, GetOwner());

		spawnUtils::DetonateBombAt(m_CachedGrid, *ge::SceneManager::GetInstance().GetCurrentActiveScene(),
			GetOwner()->GetComponent<ge::Transform>()->GetWorldPosition(),
			m_ExplosionArmLength, m_ExplosionSheetsRef, m_ExplosionLifetime);

		GetOwner()->MarkForDeletion();
	}
}

void bombGame::BombComponent::ForceDetonate() noexcept
{
	m_WindupTimer = m_WindUpDuration;
}
