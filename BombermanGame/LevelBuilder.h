#pragma once
#include "LevelLoader.h"
#include "Structs.h"

#include <optional>
#include <vector>
#include <utility>

namespace ge
{
	class Scene;
	class GameObject;
	class SpriteSheet;
	class Texture2D;
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

	// Generated the Dynamic parts of the level - breakable walls, exit, powerups
	// and STORES reference to their Game Objects and/or cooridnates
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
		std::optional<glm::vec2> GetMidGridTilePointAt(int col, int row);

		glm::vec3 GetLevelTopLeft() const noexcept;
		levelLoader::LevelLayout GetLevelLayout() const noexcept;
		float GetTileSize() const noexcept;

		// Pushes a GO to the dynamic wall container
		void RegisterBreakableAt(int col, int row, ge::GameObject* go);
		// Simply returns nullptr if no dynamic wall exists here
		ge::GameObject* GetBreakableWallAt(int col, int row) const noexcept;
		// Sets the dynamic wall from the container to nullptr
		void ClearBreakableAt(int col, int row);

		void MarkExitLocationAt(int col, int row);
		std::pair<int, int> GetExitLocation() const noexcept;
		bool IsExitTile(int col, int row) const noexcept;
		void RegisterExit(ge::GameObject* go);
		ge::GameObject* GetExitObject() const noexcept;

		// Small index helper
		int ToIndex(int col, int row) const noexcept;

		// Returns true if the tile is of type Wall or if a Breakable wall exists on this tile
		bool IsBlocked(const GridTile& tile) const noexcept;

	private:
		levelLoader::LevelLayout m_LevelLayout;
		glm::vec3 m_LevelTopLeftPos;
		float m_TileSize;

		std::vector<ge::GameObject*> m_DynamicWalls;
		ge::GameObject* m_ExitGO;
		std::pair<int, int> m_ExitCoords{ -1, -1 };
	};

	namespace levelBuilder
	{
		void BuildStaticGeometry(ge::Scene& scene, const LevelGrid& grid);
		// Generate and push the dynamic walls to the scene and store them in the grid as a reference
		void GenerateDynamicObjects(ge::Scene& scene, LevelGrid& grid,
			ge::SpriteSheet* breakableWallSheet, ge::Texture2D* exitDoorTexture, /*const std::vector<ge::Texture2D*> powerupTextures,*/
			int breakableWallRandomnessIndex);
	}
}