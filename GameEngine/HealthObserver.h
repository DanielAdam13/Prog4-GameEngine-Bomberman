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
	class HealthObserver final : public Observer
	{
	public:
		HealthObserver() = default;
		virtual ~HealthObserver() override = default;

		virtual void Notify(EventId event, ge::GameObject* sourceObject) override;

		void RegisterPlayer(ge::GameObject* player, ge::TextComponent* connectedText);

	private:
		std::unordered_map<ge::GameObject*, ge::TextComponent*> m_HealthDisplaysMap{};
	};
}