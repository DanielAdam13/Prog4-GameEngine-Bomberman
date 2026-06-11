#include "GameStateMachine.h"
#include "PlayerModeState.h"

#include <utility>
#include <memory>

void bombGame::GameStateMachine::SetInitialState(BombermanGame& game)
{
	m_CurrentGameState = std::make_unique<PlayerModeState>(game);
	if (m_CurrentGameState)
		m_CurrentGameState->OnEnter();
}

void bombGame::GameStateMachine::RequestStateTransition(std::unique_ptr<GameState> nextState)
{
	m_PendingGameState = std::move(nextState);
	m_TransitionPending = true;
}

void bombGame::GameStateMachine::Update(float deltaTime)
{
	// Do actual transition
	if (m_TransitionPending)
	{
		if (m_CurrentGameState)
			m_CurrentGameState->OnExit();

		m_CurrentGameState = std::move(m_PendingGameState);
		m_TransitionPending = false;

		if (m_CurrentGameState)
			m_CurrentGameState->OnEnter();
	}

	if (!m_CurrentGameState)
		return;

	if (auto nextState = m_CurrentGameState->Update(deltaTime))
	{
		RequestStateTransition(std::move(nextState));
	}
}

void bombGame::GameStateMachine::FixedUpdate(float fixedDeltaSec)
{
	if (m_CurrentGameState)
		m_CurrentGameState->FixedUpdate(fixedDeltaSec);
}
