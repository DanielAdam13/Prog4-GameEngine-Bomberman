#pragma once
#include "BaseEnemyState.h"

#include <glm/glm.hpp>

namespace ge
{
	class GameObject;
	class Transform;
}

namespace bombGame
{
	class EnemyComponent;

	class ChaseState final : public EnemyState
	{
	public:
		ChaseState(ge::GameObject* pTargetPtr);
		virtual ~ChaseState() = default;
		ChaseState(const ChaseState&) = delete;
		ChaseState& operator=(const ChaseState&) = delete;
		ChaseState(ChaseState&&) = delete;
		ChaseState& operator=(ChaseState&&) = delete;

		virtual void OnEnter() override;
		virtual void OnUpdate(float) override;
		virtual void OnExit() override;

	private:
		ge::GameObject* m_CurrentTarget{ nullptr }; // ref
		glm::vec3 m_DirectionToClosest{};

		float m_RefreshClosestTimer{ 0.f };
		static inline constexpr float m_RefreshClosestInterval{ 0.25f };
		
	};
}