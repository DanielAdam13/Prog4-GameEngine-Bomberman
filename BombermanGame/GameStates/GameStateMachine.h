#pragma once
#include "GameState.h"

#include <memory>

namespace bombGame
{
	class GameStateMachine final
	{
	public:
		GameStateMachine() = default;
		~GameStateMachine() = default;

		void SetInitialState(std::unique_ptr<GameState> initialState);
		void RequestStateTransition(std::unique_ptr<GameState> nextState);

		void Update(float deltaTime);
		void FixedUpdate(float fixedDeltaSec);

	private:
		std::unique_ptr<GameState> m_CurrentGameState;
		std::unique_ptr<GameState> m_PendingGameState;

		bool m_TransitionPending;
	};
}