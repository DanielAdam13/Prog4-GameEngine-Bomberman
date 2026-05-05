#pragma once

#include "StateMachine/State.h"

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	class WanderState final : public ge::State
	{
	public:
		WanderState(ge::GameObject* pTargetPtr);
		virtual ~WanderState() = default;
		WanderState(const WanderState&) = delete;
		WanderState& operator=(const WanderState&) = delete;
		WanderState(WanderState&&) = delete;
		WanderState& operator=(WanderState&&) = delete;

		virtual void OnEnter() override {};
		virtual void OnUpdate(float) override {};
		virtual void OnExit() override {};

	private:
	};
}