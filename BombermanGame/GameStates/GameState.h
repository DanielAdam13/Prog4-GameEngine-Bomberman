#pragma once

#include <memory>
#include <utility>

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

		// For UI Selection logic. Used only two times.
		// Kind of a bad abstraction but I don't have enough use cases...
		virtual void MoveHover(std::pair<int, int>) {}
		virtual void ConfirmCurrentSelection() {}

	protected:
		BombermanGame& GetBombermanGame() noexcept;
		const GameSession& GetCachedGameSession() const noexcept;

	private:
		BombermanGame& m_Game;

	};
}