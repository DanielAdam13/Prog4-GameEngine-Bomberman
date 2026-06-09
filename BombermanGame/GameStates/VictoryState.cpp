#include "VictoryState.h"
#include "MainMenuState.h"
#include "BombermanGame.h"
#include "SoundIds.h"

bombGame::VictoryState::VictoryState(BombermanGame& game, float duration)
	:GameState::GameState(game),
	m_VictoryScreenDuration{ duration }
{
}

void bombGame::VictoryState::OnEnter()
{
	GetBombermanGame().GetStoredSoundSystem()->Play(SoundIds::StageWon, 0.3f, ge::SoundCategory::Music);
}

void bombGame::VictoryState::OnExit()
{
	GetBombermanGame().GetSoundManager().StopAllSounds();
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
