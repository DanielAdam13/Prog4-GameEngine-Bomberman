#pragma once

#include "BaseEnemyState.h"

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	class RunState final : public EnemyState
	{
	public:
		RunState(ge::GameObject* pTargetPtr);
		virtual ~RunState() = default;
		RunState(const RunState&) = delete;
		RunState& operator=(const RunState&) = delete;
		RunState(RunState&&) = delete;
		RunState& operator=(RunState&&) = delete;

		virtual void OnEnter() override {};
		virtual void OnExit() override {};
		virtual std::unique_ptr<EnemyState> OnUpdate(float) override;

	private:
	};
}