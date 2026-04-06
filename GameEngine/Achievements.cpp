#include "Achievements.h"
#include "GameObject.h"
#include "GameEvents.h"

using namespace bombGame;

void AchievementsManager::Notify(int eventId, ge::GameObject*)
{
	switch (static_cast<EventId>(eventId))
	{
	case bombGame::PLAYER_DIED:
		if (m_NobodyDiedYet)
		{
			Unlock(AchievementTypes::FirstBlood);
			m_NobodyDiedYet = false;
		}
		break;
	case bombGame::GAME_OVER:
		Unlock(AchievementTypes::GameWon);
		break;
	default:
		break;
	}
}

void AchievementsManager::Unlock(AchievementTypes achievement)
{
	m_UnlockedAchievements.push_back(achievement);
}
