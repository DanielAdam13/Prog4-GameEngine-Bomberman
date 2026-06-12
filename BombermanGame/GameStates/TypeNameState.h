#pragma once
#include "GameState.h"
#include "ISelectableNavigator.h"

#include <vector>
#include <utility>
#include <memory>
#include <array>

namespace bombGame
{
	class BombermanGame;
	class SelectableTextComponent;

	class TypeNameState final : public GameState, public ge::ISelectableNavigator
	{
	public:
		TypeNameState(BombermanGame& game);

		virtual void OnEnter() override;
		virtual void OnExit() override;

		virtual std::unique_ptr<GameState> Update(float) override { return nullptr; }
		virtual void FixedUpdate(float) override {}

		// Methods called from commands
		virtual void MoveHover(std::pair<int, int> delta) override;
		virtual void ConfirmCurrentSelection() override;

	private:
		std::vector<SelectableTextComponent*> m_Selectables; // Ref
		size_t m_CurrentHoverIndex{ 0 };

		static constexpr size_t LetterCount{ 4 };
		static constexpr size_t ConfirmSlotIndex{ 4 }; // last index
		std::array<char, LetterCount> m_Letters{ 'A', 'A', 'A', 'A' };

		// Small helper to check if slot is not confirmSlot
		bool IsLetterSlot(size_t index) const;
	};
}