#include "HealthDisplayComponent.h"

#include "GameObject.h"
#include "Components/TextComponent.h"
#include "PlayerComponent.h"

#include "GameEvents.h"

#include "Components/HealthComponent.h"
#include <string>

#include <cassert>

using namespace bombGame;

HealthDisplayComponent::HealthDisplayComponent(ge::GameObject* owner, ge::GameObject* trackedPlayer)
	:Component::Component(owner),
	m_pTrackedPlayer{ trackedPlayer },
	m_pCachedText{ GetOwner()->GetComponent<ge::TextComponent>() }
{
	assert(m_pCachedText && "HealthDisplayComponent requires a TextComponent on the same GO");

	auto* playerComp = m_pTrackedPlayer->GetComponent<bombGame::PlayerComponent>();
	assert(playerComp && "HealthDisplayComponent: tracked GO has no PlayerComponent");

	playerComp->GetDamageEvent().AddObserver(this);
	playerComp->GetDeadEvent().AddObserver(this);

	RefreshText();
}

HealthDisplayComponent::~HealthDisplayComponent()
{
	// Unregistering observers - Subject holds raw pointers
	/*if (auto* playerComp{ m_pTrackedPlayer->GetComponent<bombGame::PlayerComponent>() })
	{
		playerComp->GetDamageEvent().RemoveObserver(this);
		playerComp->GetDeadEvent().RemoveObserver(this);
	}*/
}

void HealthDisplayComponent::Notify(int eventId, ge::GameObject*)
{
	switch (static_cast<EventId>(eventId))
	{
	case EventId::PLAYER_LOST_HEALTH:
	case EventId::PLAYER_DIED:
		RefreshText();
		break;
	default:
		break;
	}
}

void HealthDisplayComponent::RefreshText()
{
	auto* healthComp{ m_pTrackedPlayer->GetComponent<ge::HealthComponent>() };

	if (!healthComp || !m_pCachedText) return;

	m_pCachedText->SetText(m_pTrackedPlayer->GetName() + " HP: " + std::to_string(healthComp->GetCurrentHealth()));
}
