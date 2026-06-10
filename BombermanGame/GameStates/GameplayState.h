#pragma once
#include "GameState.h"
#include "LevelGrid.h"
#include "Camera.h"
#include "Observer.h" // For enemy death

#include <memory>
#include <vector>

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	class BombermanGame;

	class GameplayGameState final : public GameState, public ge::IObserver
	{
	public:
		GameplayGameState(BombermanGame& game);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float deltaTime) override;
		virtual void FixedUpdate(float) override {}

		virtual void Notify(int, ge::GameObject*) override;

	private:
		std::unique_ptr<LevelGrid> m_LevelGrid; // Owned, exists for the whole gameplay state
		std::unique_ptr<ge::Camera> m_GameplayCamera; // Owned

		std::vector<ge::GameObject*> m_TrackedPlayers; // Cached Ref

		int m_RemainingEnemyCount{ 0 };
		bool m_StageCleared{ true };

		// Private Helper
		bool IsAnyPlayerOnExit() const noexcept;

		static inline constexpr float GameplayTime{ 180.f };
		float m_GameplayTimer{ GameplayTime };
	};
}