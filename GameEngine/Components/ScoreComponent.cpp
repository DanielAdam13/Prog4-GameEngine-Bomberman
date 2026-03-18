#include "ScoreComponent.h"

using namespace bombGame;

ScoreComponent::ScoreComponent(ge::GameObject* pOwnerPtr, int startingScore)
	:ge::Component::Component(pOwnerPtr),
	m_CurrentScore{ startingScore }
{
}

void ScoreComponent::ChangeScore(int scoreDiff)
{
	m_CurrentScore += scoreDiff;

	if (m_OnScoreChange)
		m_OnScoreChange();
}

int ScoreComponent::GetCurrentScore() const noexcept
{
	return m_CurrentScore;
}

void ScoreComponent::SetOnScoreChange(std::function<void()> callback)
{
	m_OnScoreChange = callback;
}
