#pragma once

#include <functional>

namespace ge
{
	class State;

	class Transition final
	{
	public:
		Transition(State* inFrom, State* inTo, std::function<bool()> inCondition);

		State* GetFrom() const noexcept;
		State* GetTo() const noexcept;
		bool Evaluate() const noexcept;

	private:
		State* m_From{ nullptr };
		State* m_To{ nullptr };
		std::function<bool()> m_Condition;
	};
}