#include "HealthObserver.h"
#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/TextComponent.h"

using namespace bombGame;

void HealthObserver::Notify(EventId event, ge::GameObject* sourceObject)
{
	if (event == EventId::PLAYER_LOST_HEALTH)
	{
		auto it{ m_HealthDisplaysMap.find(sourceObject) };
		if (it != m_HealthDisplaysMap.end())
		{
			const int objectHp{ sourceObject->GetComponent<ge::HealthComponent>()->GetCurrentHealth() };
			it->second->SetText(sourceObject->GetName() + " HP: " + std::to_string(objectHp));
		}
	}
}

void HealthObserver::RegisterPlayer(ge::GameObject* player, ge::TextComponent* connectedText)
{
	m_HealthDisplaysMap[player] = connectedText;
}
