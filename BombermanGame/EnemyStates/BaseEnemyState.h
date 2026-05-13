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

	class EnemyState : public ge::State
	{
	public:
		EnemyState(ge::GameObject* pTargetPtr);
		virtual ~EnemyState() = default;
		EnemyState(const EnemyState&) = delete;
		EnemyState& operator=(const EnemyState&) = delete;
		EnemyState(EnemyState&&) = delete;
		EnemyState& operator=(EnemyState&&) = delete;

	protected:
		ge::GameObject* FindClosestPlayerInRange() const;

		EnemyComponent* GetSourceEnemyComponent() const noexcept;
		ge::Transform* GetSourceTransform() const noexcept;
		
	private:
		EnemyComponent* m_pEnemyComponent{ nullptr }; // Cached ref
		ge::Transform* m_pSourceTransform{ nullptr }; // Cahced ref

	};
}