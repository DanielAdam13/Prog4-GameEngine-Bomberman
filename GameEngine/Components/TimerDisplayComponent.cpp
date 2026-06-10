#include "TimerDisplayComponent.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "EngineEvents.h"

#include <string>

ge::TimerDisplayComponent::TimerDisplayComponent(GameObject* pOwnerPtr, float startTime, float goalTime, TimerType type, float changeInterval)
	:Component::Component(pOwnerPtr),
	m_ActualTimer{ startTime },
	m_GoalTime{ goalTime },
	m_ChangeTimer{ changeInterval },
	m_ChangeInterval{ changeInterval },
	m_TimerType{ type }
{
}

void ge::TimerDisplayComponent::UpdateComponent(float deltaTime)
{
	if (!m_pTextComponent)
		m_pTextComponent = GetOwner()->GetComponent<TextComponent>();

	if (m_ReachedGoal)
		return;

	switch (m_TimerType)
	{
	case ge::TimerDisplayComponent::TimerType::Incrementing:
		m_ActualTimer += deltaTime;
		if (m_ActualTimer >= m_GoalTime)
		{
			m_ReachedGoalEvent.NotifyObservers(EngineEventId::TIMER_REACHED_GOAL, GetOwner());
			m_ReachedGoal = true;
		}
		break;
	case ge::TimerDisplayComponent::TimerType::Decrementing:
		m_ActualTimer -= deltaTime;
		if (m_ActualTimer <= m_GoalTime)
		{
			m_ReachedGoalEvent.NotifyObservers(EngineEventId::TIMER_REACHED_GOAL, GetOwner());
			m_ReachedGoal = true;
		}
		break;
	}

	m_ChangeTimer += deltaTime;
	if (m_ChangeTimer >= m_ChangeInterval)
	{
		if (m_pTextComponent)
		{
			m_pTextComponent->SetText("TIME " + std::to_string(static_cast<int>(m_ActualTimer)));
		}

		m_ChangeTimer = 0.f;
	}
}