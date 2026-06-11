#include "ScoreComponent.h"
#include "EngineEvents.h"

using namespace ge;

ScoreComponent::ScoreComponent(ge::GameObject* pOwnerPtr, int startingScore)
	:ge::Component::Component(pOwnerPtr),
	m_CurrentScore{ startingScore }
{
}

void ScoreComponent::ChangeScore(int scoreDiff)
{
	m_CurrentScore += scoreDiff;

	m_OnScoreChangedEvent.NotifyObservers(events::EngineEventId::SCORE_CHANGED, GetOwner());
}

int ScoreComponent::GetCurrentScore() const noexcept
{
	return m_CurrentScore;
}
