#include "SoundManager.h"

#include <Services/SoundSystem.h>
#include <Services/ServiceLocator.h>
#include "GameObject.h"
#include "Components/HealthComponent.h"

#include "SoundIds.h"
#include "GameEvents.h"

bombGame::SoundManager::SoundManager()
{
	RegisterMapping(GameEventId::PLAYER_DIED, SoundIds::BombermanDied, 0.15f);
	/*RegisterMapping(GameEventId::PLAYER_LOST_HEALTH, SoundIds::ExplosionBomb, 0.75f);
	RegisterMapping(GameEventId::PLAYER_SCORE_CHANGED, SoundIds::Powerup, 0.15f);*/
	RegisterMapping(GameEventId::LAY_BOMB, SoundIds::LayBomb, 0.15f);
	RegisterMapping(GameEventId::EXPLODED_BOMB, SoundIds::ExplosionBomb, 0.3f);
	RegisterMapping(GameEventId::PLAYER_MOVED_HOR, SoundIds::Step_Horizontal, 0.3f);
	RegisterMapping(GameEventId::PLAYER_MOVED_VERT, SoundIds::Step_Vertical, 0.3f);
}

void bombGame::SoundManager::Notify(int eventId, ge::GameObject* sourceObj)
{
	auto it{ m_SoundMappings.find(static_cast<GameEventId>(eventId)) };
	if (it != m_SoundMappings.end())
	{
		// Simple check to not play HURT sound when dying
		if (static_cast<GameEventId>(eventId) == GameEventId::PLAYER_LOST_HEALTH &&
			[sourceObj]() -> bool {
				if (!sourceObj)
					return true;

				auto* sourceHealth{ sourceObj->GetComponent<ge::HealthComponent>() };
				return sourceHealth && sourceHealth->GetCurrentHealth() == 0;
			}())
		{
			return;
		}

		ge::ServiceLocator::GetSoundSystem().Play(it->second.soundId, it->second.volume);
	}
}

void bombGame::SoundManager::RegisterMapping(GameEventId eventId, ge::Sound_Id soundId, float volume)
{
	m_SoundMappings.insert_or_assign(eventId, SoundMapping{ soundId, volume });
}
