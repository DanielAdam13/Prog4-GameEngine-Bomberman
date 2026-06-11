#pragma once

#include <memory>

namespace bombGame
{
	class BombermanGame;
	struct GameSession;

	class GameState
	{
	public:
		GameState(BombermanGame& game);
			
		virtual ~GameState() = default;
		GameState(const GameState&) = delete;
		GameState& operator=(const GameState&) = delete;
		GameState(GameState&&) = delete;
		GameState& operator=(GameState&&) = delete;

		virtual void OnEnter() = 0;
		virtual void OnExit() = 0;

		virtual std::unique_ptr<GameState> Update(float) = 0; // Returns potential new state
		virtual void FixedUpdate(float) = 0;

	protected:
		BombermanGame& GetBombermanGame() noexcept;
		GameSession& GetCachedGameSession() noexcept;

	private:
		BombermanGame& m_Game;

	};
}