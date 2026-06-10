#include "ScoreDisplayComponent.h"

#include "GameObject.h"
#include "Components/TextComponent.h"
#include "PlayerComponent.h"

#include "GameEvents.h"

#include "Components/ScoreComponent.h"
#include <string>

#include <cassert>

using namespace bombGame;

ScoreDisplayComponent::ScoreDisplayComponent(ge::GameObject* owner, ge::GameObject* trackedPlayer)
	:Component::Component(owner),
	m_pTrackedPlayer{ trackedPlayer },
	m_pCachedText{ GetOwner()->GetComponent<ge::TextComponent>() }
{
	assert(m_pCachedText && "ScoreDisplayComponent requires a TextComponent on the same GO");

	auto* playerComp{ m_pTrackedPlayer->GetComponent<PlayerComponent>() };
	assert(playerComp && "ScoreDisplayComponent: tracked GO has no PlayerComponent");
	playerComp->GetScoreChangeEvent().AddObserver(this);

	RefreshText();
}

void ScoreDisplayComponent::Notify(int eventId, ge::GameObject*)
{
	if (static_cast<GameEventId>(eventId) == GameEventId::PLAYER_SCORE_CHANGED)
	{
		RefreshText();
	}
}

void ScoreDisplayComponent::RefreshText()
{
	if (!m_pTrackedPlayer)
		return;

	auto* score{ m_pTrackedPlayer->GetComponent<ge::ScoreComponent>() };
	if (!score || !m_pCachedText) 
		return;

	//m_pCachedText->SetText(m_pTrackedPlayer->GetName() + " " + std::to_string(score->GetCurrentScore()));
	m_pCachedText->SetText(std::to_string(score->GetCurrentScore()));
}
