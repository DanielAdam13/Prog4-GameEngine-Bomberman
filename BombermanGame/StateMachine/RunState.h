#pragma once

#include "StateMachine/State.h"

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	class RunState final : public ge::State
	{
	public:
		RunState(ge::GameObject* pTargetPtr);
		virtual ~RunState() = default;
		RunState(const RunState&) = delete;
		RunState& operator=(const RunState&) = delete;
		RunState(RunState&&) = delete;
		RunState& operator=(RunState&&) = delete;

		virtual void OnEnter() override {};
		virtual void OnUpdate(float) override {};
		virtual void OnExit() override {};

	private:
	};
}