#include "State.h"

ge::State::State(GameObject* pTargetPtr)
	: m_TargetPtr{ pTargetPtr }
{
}

ge::GameObject* ge::State::GetTarget() const noexcept
{
	return nullptr;
}
