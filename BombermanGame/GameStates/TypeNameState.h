#pragma once
#include "GameState.h"

#include <vector>
#include <utility>
#include <memory>
#include <array>

namespace bombGame
{
	class BombermanGame;
	class SelectableTextComponent;

	class TypeNameState final : public GameState
	{
	public:
		TypeNameState(BombermanGame& game);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float) override { return nullptr; }
		virtual void FixedUpdate(float) override {}

		// Methods called from commands
		void MoveHover(std::pair<int, int> delta) override;
		void ConfirmCurrentSelection() override;

	private:
		static constexpr size_t LetterCount{ 4 };
		static constexpr size_t ConfirmSlotIndex{ 4 }; // last index

		std::array<char, LetterCount> m_Letters{ 'A', 'A', 'A', 'A' };

		std::vector<SelectableTextComponent*> m_Selectables;
		size_t m_CurrentHoverIndex{ 0 };

		// Small helper to check if slot is not confirmSlot
		bool IsLetterSlot(size_t index) const;
	};
}