#pragma once

#include <memory>

namespace ge
{
	class GameObject;
	class Transform;
}

namespace bombGame
{
	class EnemyComponent;

	class EnemyState
	{
	public:
		EnemyState(ge::GameObject* pTargetPtr);
		virtual ~EnemyState() = default;
		EnemyState(const EnemyState&) = delete;
		EnemyState& operator=(const EnemyState&) = delete;
		EnemyState(EnemyState&&) = delete;
		EnemyState& operator=(EnemyState&&) = delete;

		virtual void OnEnter() = 0;
		virtual void OnExit() = 0;

		virtual std::unique_ptr<EnemyState> OnUpdate(float) = 0; // Returns potential new state

	protected:
		ge::GameObject* FindClosestPlayerInRange() const;

		EnemyComponent* GetSourceEnemyComponent() const noexcept;
		ge::Transform* GetSourceTransform() const noexcept;
		
	private:
		EnemyComponent* m_pEnemyComponent{ nullptr }; // Cached ref
		ge::Transform* m_pSourceTransform{ nullptr }; // Cahced ref

	};
}