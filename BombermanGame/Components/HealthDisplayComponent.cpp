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

void HealthDisplayComponent::Notify(int eventId, ge::GameObject*)
{
	switch (static_cast<events::GameEventId>(eventId))
	{
	case events::GameEventId::PLAYER_LOST_HEALTH:
	case events::GameEventId::PLAYER_DIED:
		RefreshText();
		break;
	default:
		break;
	}
}

void HealthDisplayComponent::RefreshText()
{
	if (!m_pTrackedPlayer)
		return;

	auto* healthComp{ m_pTrackedPlayer->GetComponent<ge::HealthComponent>() };

	if (!healthComp || !m_pCachedText) 
		return;

	m_pCachedText->SetText("LEFT " + std::to_string(healthComp->GetCurrentHealth()));
}
