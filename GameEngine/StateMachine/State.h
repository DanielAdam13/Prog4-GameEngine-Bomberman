#pragma once

namespace ge
{
	class State
	{
	public:
		State() = default;
		virtual ~State() = default;
		State(const State&) = delete;
		State& operator=(const State&) = delete;
		State(State&&) = delete;
		State& operator=(State&&) = delete;

		virtual void OnEnter() = 0;
		virtual void OnUpdate(float) = 0;
		virtual void OnExit() = 0;

	protected:

	private:

	};
}