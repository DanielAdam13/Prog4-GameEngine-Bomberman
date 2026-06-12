#pragma once
#include "GameState.h"

#include <memory>
#include <vector>
#include <utility>

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
		virtual void MoveHover(std::pair<int, int> delta) override;
		virtual void ConfirmCurrentSelection() override;

	private:
		std::vector<SelectableTextComponent*> m_Selectables{}; // Ref
		size_t m_CurrentHoverIndex{ 0 };
	};
}