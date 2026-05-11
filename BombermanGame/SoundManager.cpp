#include "SoundManager.h"

#include <Services/SoundSystem.h>
#include <Services/ServiceLocator.h>

#include "Services/SoundIds.h"
#include "GameEvents.h"

bombGame::SoundManager::SoundManager()
{
	RegisterMapping(GameEventId::PLAYER_DIED, SoundIds::BombermanDied, 0.15f);
	RegisterMapping(GameEventId::PLAYER_LOST_HEALTH, SoundIds::ExplosionBomb, 0.75f);
	RegisterMapping(GameEventId::PLAYER_SCORE_CHANGED, SoundIds::Powerup, 0.15f);
	RegisterMapping(GameEventId::LAY_BOMB, SoundIds::LayBomb, 0.15f);
	RegisterMapping(GameEventId::EXPLODED_BOMB, SoundIds::ExplosionBomb, 0.15f);
}

void bombGame::SoundManager::Notify(int eventId, ge::GameObject*)
{
	auto it{ m_SoundMappings.find(static_cast<GameEventId>(eventId)) };
	if (it != m_SoundMappings.end())
	{
		ge::ServiceLocator::GetSoundSystem().Play(it->second.soundId, it->second.volume);
	}
}

void bombGame::SoundManager::RegisterMapping(GameEventId eventId, ge::Sound_Id soundId, float volume)
{
	m_SoundMappings.insert_or_assign(eventId, SoundMapping{ soundId, volume });
}
