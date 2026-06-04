#pragma once
#include "Structs.h"
#include "LevelLoader.h"

#include <optional>
#include <vector>
#include <glm/glm.hpp>
#include <utility>

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	struct GridTile
	{
		ge::structs::Rect gridRect;
		int gridTileIndex;
		levelLoader::TileType gridTileType;

		int col;
		int row;
	};

	// Plays the role of "Grid Tile Manager". Specifies HOW the layout is displayed.
	// Also stores pointers/references to dynamically created grid objects such as Breakable Walls or the Exit.
	class LevelGrid final
	{
	public:
		LevelGrid(const levelLoader::LevelLayout& layout, const glm::vec3& buildTopLeftPos, const float tileSize);
		~LevelGrid() = default;
		LevelGrid(const LevelGrid& other) = delete;
		LevelGrid(LevelGrid&& other) = delete;
		LevelGrid& operator=(const LevelGrid& other) = delete;
		LevelGrid& operator=(LevelGrid&& other) = delete;

		std::optional<GridTile> GetGridTileAt(const glm::vec3& worldPos) const noexcept;
		std::optional<GridTile> GetGridTileByCoord(int col, int row) const noexcept;
		std::optional<glm::vec2> GetMidGridTilePointAt(const glm::vec3& worldPos) const noexcept;
		std::optional<glm::vec2> GetMidGridTilePointByCoord(int col, int row) const noexcept;

		glm::vec3 GetLevelTopLeft() const noexcept;
		levelLoader::LevelLayout GetLevelLayout() const noexcept;
		float GetTileSize() const noexcept;

		// Asigns a GO* to an index in the dynamic wall container
		void RegisterBreakableAt(int col, int row, ge::GameObject* go);
		// Simply returns nullptr if no dynamic wall exists here
		ge::GameObject* GetBreakableWallAt(int col, int row) const noexcept;
		// Sets the dynamic wall* from the container to nullptr
		void ClearBreakableAt(int col, int row);

		void MarkExitLocationAt(int col, int row);
		std::pair<int, int> GetExitLocation() const noexcept;
		bool IsExitTile(int col, int row) const noexcept;
		void RegisterExit(ge::GameObject* go);
		ge::GameObject* GetExitObject() const noexcept;

		// Asigns a GO* to an index in the bomb container
		void RegisterBombAt(int col, int row, ge::GameObject* go);
		// Simply returns nullptr if no bomb exists here
		ge::GameObject* GetBombAt(int col, int row) const noexcept;
		// Sets the bomb* from the container to nullptr
		void ClearBombAt(int col, int row);

		// Small index helper
		int ToIndex(int col, int row) const noexcept;

		// Returns true if the tile is of type Wall or if a Breakable wall exists on this tile
		// + Or if a bomb is at the tile
		bool IsBlocked(const GridTile& tile) const noexcept;

	private:
		levelLoader::LevelLayout m_LevelLayout;
		glm::vec3 m_LevelTopLeftPos;
		float m_TileSize;

		std::vector<ge::GameObject*> m_DynamicWalls;
		ge::GameObject* m_ExitGO;
		std::pair<int, int> m_ExitCoords{ -1, -1 };
		std::vector<ge::GameObject*> m_CurrentlyActiveBombs;
	};
}