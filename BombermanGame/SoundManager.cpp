#include "SoundManager.h"

#include <Services/SoundSystem.h>
#include <Services/ServiceLocator.h>

#include "Services/SoundIds.h"
#include "GameEvents.h"

bombGame::SoundManager::SoundManager()
{
	RegisterMapping(EventId::PLAYER_DIED, SoundIds::BombermanDied, 0.15f);
	RegisterMapping(EventId::PLAYER_LOST_HEALTH, SoundIds::ExplosionBomb, 0.75f);
	RegisterMapping(EventId::PLAYER_SCORE_CHANGED, SoundIds::Powerup, 0.15f);
}

void bombGame::SoundManager::Notify(int eventId, ge::GameObject*)
{
	auto it{ m_SoundMappings.find(static_cast<EventId>(eventId)) };
	if (it != m_SoundMappings.end())
	{
		ge::ServiceLocator::GetSoundSystem().Play(it->second.soundId, it->second.volume);
	}
}

void bombGame::SoundManager::RegisterMapping(EventId eventId, ge::Sound_Id soundId, float volume)
{
	m_SoundMappings.insert_or_assign(eventId, SoundMapping{ soundId, volume });
}
