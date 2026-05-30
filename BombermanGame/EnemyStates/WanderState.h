#pragma once

#include "BaseEnemyState.h"

#include <glm/glm.hpp>
#include <array>

namespace ge
{
	class GameObject;
	class Transform;
}

namespace bombGame
{
	class EnemyComponent;
	struct GridTile;

	class WanderState final : public EnemyState
	{
	public:
		WanderState(ge::GameObject* pTargetPtr);
		virtual ~WanderState() = default;
		WanderState(const WanderState&) = delete;
		WanderState& operator=(const WanderState&) = delete;
		WanderState(WanderState&&) = delete;
		WanderState& operator=(WanderState&&) = delete;

		virtual void OnEnter() override;
		virtual void OnExit() override {}
		virtual std::unique_ptr<EnemyState> OnUpdate(float) override;

		// Wander State: picks a random walkable direction
		virtual glm::vec3 ChooseDirectionAtIntersection(const GridTile& currentTile) override;

	private:
		float m_WanderTimer{ 0.f };

		float m_PlayerCheckTimer{ 0.f };
		static constexpr inline float m_PlayerCheckInterval{ 0.25f };
	};
}