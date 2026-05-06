#pragma once

#include "StateMachine/State.h"

namespace ge
{
	class GameObject;
	class Transform;
}

namespace bombGame
{
	class EnemyComponent;

	class ChaseState final : public ge::State
	{
	public:
		ChaseState(ge::GameObject* pTargetPtr);
		virtual ~ChaseState() = default;
		ChaseState(const ChaseState&) = delete;
		ChaseState& operator=(const ChaseState&) = delete;
		ChaseState(ChaseState&&) = delete;
		ChaseState& operator=(ChaseState&&) = delete;

		virtual void OnEnter() override {};
		virtual void OnUpdate(float) override;
		virtual void OnExit() override {};

	private:
		ge::Transform* m_pSourceTransform{ nullptr }; // Cached ref
		EnemyComponent* m_pEnemyController{ nullptr }; // Cached ref
	};
}