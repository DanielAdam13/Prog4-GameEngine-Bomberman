#include "VictoryState.h"
#include "MainMenuState.h"
#include "BombermanGame.h"

bombGame::VictoryState::VictoryState(BombermanGame& game, float duration)
	:GameState::GameState(game),
	m_VictoryScreenDuration{ duration }
{
}

void bombGame::VictoryState::OnEnter()
{
}

void bombGame::VictoryState::OnExit()
{
}

std::unique_ptr<bombGame::GameState> bombGame::VictoryState::Update(float deltaTime)
{
	m_VictoryTimer += deltaTime;
	if (m_VictoryTimer >= m_VictoryScreenDuration)
	{
		GetBombermanGame().ResetGameplayStageIndex();
		return std::make_unique<MainMenuGameState>(GetBombermanGame());
	}

	return nullptr;
}
