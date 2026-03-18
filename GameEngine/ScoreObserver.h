#pragma once

#include "Observer.h"
#include <unordered_map>

namespace ge
{
	class GameObject;
	class TextComponent;
}

namespace bombGame
{
	class ScoreObserver final : public Observer
	{
	public:
		ScoreObserver() = default;
		virtual ~ScoreObserver() override = default;

		virtual void Notify(EventId event, ge::GameObject* sourceObject) override;

		void RegisterPlayer(ge::GameObject* player, ge::TextComponent* connectedScoreText);

	private:
		std::unordered_map<ge::GameObject*, ge::TextComponent*> m_ScoreDisplaysMap{};
	};
}