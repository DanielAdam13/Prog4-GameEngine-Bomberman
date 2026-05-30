#include "RunState.h"

bombGame::RunState::RunState(ge::GameObject* pTargetPtr)
	:EnemyState::EnemyState(pTargetPtr)
{
}

std::unique_ptr<bombGame::EnemyState> bombGame::RunState::OnUpdate(float)
{
	return std::unique_ptr<EnemyState>();
}
