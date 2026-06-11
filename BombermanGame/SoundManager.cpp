#include "SoundManager.h"

#include <Services/SoundSystem.h>
#include <Services/ServiceLocator.h>
#include "GameObject.h"
#include "Components/HealthComponent.h"

#include "SoundIds.h"
#include "GameEvents.h"

bombGame::SoundManager::SoundManager()
{
	RegisterMapping(events::GameEventId::PLAYER_DIED, SoundIds::BombermanDied, 0.15f);
	/*RegisterMapping(GameEventId::PLAYER_LOST_HEALTH, SoundIds::ExplosionBomb, 0.75f);
	RegisterMapping(GameEventId::PLAYER_SCORE_CHANGED, SoundIds::Powerup, 0.15f);*/
	RegisterMapping(events::GameEventId::LAY_BOMB, SoundIds::LayBomb, 0.15f);
	RegisterMapping(events::GameEventId::EXPLODED_BOMB, SoundIds::ExplosionBomb, 0.3f);
	RegisterMapping(events::GameEventId::PLAYER_MOVED_HOR, SoundIds::StepHorizontal, 0.3f);
	RegisterMapping(events::GameEventId::PLAYER_MOVED_VERT, SoundIds::StepVertical, 0.3f);
	RegisterMapping(events::GameEventId::POWERUP_PICKED_UP, SoundIds::Powerup, 0.2f);
}

void bombGame::SoundManager::Notify(int eventId, ge::GameObject*)
{
	auto it{ m_SoundMappings.find(static_cast<events::GameEventId>(eventId)) };
	if (it != m_SoundMappings.end())
	{
		ge::ServiceLocator::GetSoundSystem().Play(it->second.soundId, it->second.volume);
	}
}

void bombGame::SoundManager::RegisterMapping(events::GameEventId eventId, ge::Sound_Id soundId, float volume)
{
	m_SoundMappings.insert_or_assign(eventId, SoundMapping{ soundId, volume });
}

void bombGame::SoundManager::StopAllSounds()
{
	ge::ServiceLocator::GetSoundSystem().StopAll();
}

void bombGame::SoundManager::SetMuted(bool muted)
{
	ge::ServiceLocator::GetSoundSystem().SetMuted(muted);
}

bool bombGame::SoundManager::IsMuted() const
{
	return ge::ServiceLocator::GetSoundSystem().IsMuted();
}
