#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <utility>
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
	class LevelGrid;

	namespace MovementDirections
	{
		constexpr std::pair<int, int> Up{ 0, -1 };
		constexpr std::pair<int, int> Right{ 1, 0 };
		constexpr std::pair<int, int> Down{ 0, 1 };
		constexpr std::pair<int, int> Left{ -1, 0 };
	}

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

		// Derived States need to override.
		// Wander State: picks a random walkable direction
		// Chase State: picks the direction that minimizes the distance to player
		virtual glm::vec3 ChooseDirectionAtIntersection(const GridTile& currentTile) = 0;

	protected:
		// Subclass Sandbox pattern kinda
		ge::GameObject* FindClosestPlayerInRange() const;

		EnemyComponent* GetSourceEnemyComponent() const noexcept;
		ge::Transform* GetSourceTransform() const noexcept;

		// Subclass Sandbox pattern kinda
		std::vector<glm::vec3> CollectWalkableNeighbors(const LevelGrid& grid, const GridTile& currentTile, const glm::vec3& currentDir) const;

		static constexpr inline std::array<std::pair<int, int>, 4> m_PossibleDirections{
			MovementDirections::Up, MovementDirections::Right, MovementDirections::Down, MovementDirections::Left };
		
	private:
		EnemyComponent* m_CachedEnemyComponent{ nullptr }; // Cached ref
		ge::Transform* m_CachedSourceTransform{ nullptr }; // Cahced ref

	};
}