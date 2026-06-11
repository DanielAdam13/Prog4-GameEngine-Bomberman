#pragma once
#include "GameState.h"

#include <memory>
#include <vector>

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	class BombermanGame;
	class SelectableTextComponent;

	class PlayerModeState final : public GameState
	{
	public:
		PlayerModeState(BombermanGame& game);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float) override { return nullptr; }
		virtual void FixedUpdate(float) override {}

		// Methods called from commands
		void MoveHover(int delta);
		void ConfirmCurrentSelection();

	private:
		std::vector<SelectableTextComponent*> m_Selectables;
		size_t m_CurrentHoverIndex{ 0 };
	};
}