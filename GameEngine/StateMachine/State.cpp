#include "State.h"

ge::State::State(GameObject* pTargetPtr)
	: m_TargetPtr{ pTargetPtr }
{
}

ge::GameObject* ge::State::GetSource() const noexcept
{
	return m_TargetPtr;
}
