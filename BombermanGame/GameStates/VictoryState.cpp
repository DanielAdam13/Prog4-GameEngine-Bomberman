#include "VictoryState.h"
#include "MainMenuState.h"
#include "BombermanGame.h"

bombGame::VictoryState::VictoryState(BombermanGame& game)
	:GameState::GameState(game)
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
	victoryTimer += deltaTime;
	if (victoryTimer >= victoryScreenDuration)
	{
		GetBombermanGame().ResetGameplayStageIndex();
		return std::make_unique<MainMenuGameState>(GetBombermanGame());
	}

	return nullptr;
}
