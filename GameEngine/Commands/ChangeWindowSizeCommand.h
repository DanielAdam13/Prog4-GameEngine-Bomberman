#pragma once
#include "Commands/Command.h"
#include <utility>

namespace ge
{
	// Engine Command
	class ChangeWindowSizeCommand final : public ge::Command
	{
	public:
		ChangeWindowSizeCommand(const int changeSizeX, const int changeSizeY);
		~ChangeWindowSizeCommand() override = default;

		virtual void Execute(float) override;

	private:
		std::pair<int, int> m_OriginalSize;
		std::pair<int, int> m_ChangedSize;

		bool m_IsInChanged;
	};
}