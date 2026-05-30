#pragma once
#include "BaseEnemyState.h"

#include <glm/glm.hpp>
#include <memory>

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
		virtual void OnExit() override;
		virtual std::unique_ptr<EnemyState> OnUpdate(float) override;

		// Chase State: picks the direction that minimizes the distance to player
		virtual glm::vec3 ChooseDirectionAtIntersection(const GridTile& currentTile) override;

	private:
		ge::GameObject* m_CurrentTarget{ nullptr }; // Cached ref
		glm::vec3 m_DirectionToClosest{};

		float m_RefreshClosestTimer{ 0.f };
		static inline constexpr float m_RefreshClosestInterval{ 0.25f };
		
	};
}