#include "Transition.h"

#include "State.h"

#include <utility>

ge::Transition::Transition(State* inFrom, State* inTo, std::function<bool()> inCondition)
	:m_From{ inFrom },
	m_To{ inTo },
	m_Condition{ std::move(inCondition) }
{
}

ge::State* ge::Transition::GetFrom() const noexcept
{
	return m_From;
}

ge::State* ge::Transition::GetTo() const noexcept
{
	return m_To;
}

bool ge::Transition::Evaluate() const noexcept
{
	return m_Condition && m_Condition();
}
