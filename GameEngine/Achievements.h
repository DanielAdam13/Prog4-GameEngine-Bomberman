#pragma once
#include "Observer.h"
#include <vector>
#include "GameEvents.h"

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	class AchievementsManager final : public ge::IObserver
	{
	public:
		AchievementsManager() = default;
		virtual ~AchievementsManager() override = default;

		virtual void Notify(int eventId, ge::GameObject* sourceObject) override;

		enum class AchievementTypes
		{
			FirstBlood,
			GameWon
		};

	private:
		void Unlock(AchievementTypes achievement);

		std::vector<AchievementTypes> m_UnlockedAchievements{};

		bool m_NobodyDiedYet{ true };
	};
}