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
	namespace MovementDirections
	{
		constexpr glm::vec3 Up{ 0.f, -1.f, 0.f };
		constexpr glm::vec3 Right{ 1.f, 0.f, 0.f };
		constexpr glm::vec3 Down{ 0.f, 1.f, 0.f };
		constexpr glm::vec3 Left{ -1.f, 0.f, 0.f };
	}

	class EnemyComponent;

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
		virtual void OnUpdate(float) override;
		virtual void OnExit() override;

	private:
		glm::vec3 m_CurrentWanderDirection;
		static constexpr inline std::array<glm::vec3, 4> m_PossibleDirections{
			MovementDirections::Up, MovementDirections::Right, MovementDirections::Down, MovementDirections::Left };

		float m_WanderTimer{ 0.f };
		static constexpr inline float DirectionSwitchTimer{ 1.5f };

		float m_PlayerCheckTimer{ 0.f };
		static constexpr inline float m_PlayerCheckInterval{ 0.25f };
	};
}