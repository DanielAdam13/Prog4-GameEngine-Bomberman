#include "ScoreDisplayComponent.h"

#include "GameObject.h"
#include "TextComponent.h"
#include "PlayerComponent.h"

#include "GameEvents.h"

#include "ScoreComponent.h"
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

ScoreDisplayComponent::~ScoreDisplayComponent()
{
	// Unregistering observers - Subject holds raw pointers
	if (auto* playerComp{ m_pTrackedPlayer->GetComponent<bombGame::PlayerComponent>() })
	{
		playerComp->GetScoreChangeEvent().RemoveObserver(this);
	}
}

void ScoreDisplayComponent::Notify(int eventId, ge::GameObject*)
{
	if (static_cast<EventId>(eventId) == EventId::PLAYER_SCORE_CHANGED)
	{
		RefreshText();
	}
}

void ScoreDisplayComponent::RefreshText()
{
	auto* score{ m_pTrackedPlayer->GetComponent<ge::ScoreComponent>() };
	if (!score || !m_pCachedText) return;

	m_pCachedText->SetText("Score: " + std::to_string(score->GetCurrentScore()));
}
