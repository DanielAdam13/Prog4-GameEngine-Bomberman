#include "ScoreObserver.h"
#include "GameObject.h"
#include "Components/ScoreComponent.h"
#include "Components/TextComponent.h"

using namespace bombGame;

void ScoreObserver::Notify(EventId event, ge::GameObject* sourceObject)
{
	if (event == EventId::PLAYER_SCORE_CHANGED)
	{
		auto it{ m_ScoreDisplaysMap.find(sourceObject) };
		if (it != m_ScoreDisplaysMap.end())
		{
			const int score{ sourceObject->GetComponent<ScoreComponent>()->GetCurrentScore() };
			it->second->SetText("Score: " + std::to_string(score));
		}
	}
}

void ScoreObserver::RegisterPlayer(ge::GameObject* player, ge::TextComponent* connectedScoreText)
{
	m_ScoreDisplaysMap[player] = connectedScoreText;
}
