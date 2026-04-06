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
	class AchievementsObserver final : public ge::IObserver
	{
	public:
		AchievementsObserver() = default;
		virtual ~AchievementsObserver() override = default;

		//virtual void Notify(EventId event, ge::GameObject* sourceObject) override;

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