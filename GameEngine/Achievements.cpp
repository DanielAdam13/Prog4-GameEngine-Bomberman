#include "Achievements.h"
#include "GameObject.h"

using namespace bombGame;

//void AchievementsObserver::Notify(EventId event, ge::GameObject*)
//{
//	switch (event)
//	{
//	case bombGame::PLAYER_DIED:
//		if (m_NobodyDiedYet)
//		{
//			Unlock(AchievementTypes::FirstBlood);
//			m_NobodyDiedYet = false;
//		}
//		break;
//	case bombGame::GAME_OVER:
//		Unlock(AchievementTypes::GameWon);
//		break;
//	default:
//		break;
//	}
//}

void AchievementsObserver::Unlock(AchievementTypes achievement)
{
	m_UnlockedAchievements.push_back(achievement);
}
