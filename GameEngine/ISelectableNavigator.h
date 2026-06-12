#pragma once

#include <utility>

namespace ge
{
	// Interface for UI logic
	class ISelectableNavigator
	{
	public:
		virtual ~ISelectableNavigator() = default;
		virtual void MoveHover(std::pair<int, int> delta) = 0;
		virtual void ConfirmCurrentSelection() = 0;
	};
}