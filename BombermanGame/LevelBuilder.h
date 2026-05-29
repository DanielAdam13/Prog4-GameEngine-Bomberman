#pragma once
#include "LevelLoader.h"
#include "Structs.h"

#include <optional>
#include <vector>

namespace ge
{
	class Scene;
	class Texture2D;
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

		glm::vec3 GetLevelTopLeft() const noexcept;
		levelLoader::LevelLayout GetLevelLayout() const noexcept;
		float GetTileSize() const noexcept;

		// Pushes a GO to the dynamic wall container
		void RegisterBreakableAt(int col, int row, ge::GameObject* go);
		// Simply returns nullptr if no dynamic wall exists here
		ge::GameObject* GetBreakableWallAt(int col, int row) const noexcept;
		// Sets the dynamic wall from the container to nullptr
		void ClearBreakableAt(int col, int row);

		// Small index helper
		int ToIndex(int col, int row) const noexcept;

	private:
		levelLoader::LevelLayout m_LevelLayout;
		glm::vec3 m_LevelTopLeftPos;
		float m_TileSize;

		std::vector<ge::GameObject*> m_DynamicWalls;
	};

	namespace levelBuilder
	{
		void BuildStaticGeometry(ge::Scene& scene, const LevelGrid& grid);
		// Generate and push the dynamic walls to the scene and store them in the grid as a reference
		void GenerateDynamicObjects(ge::Scene& scene, LevelGrid& grid,
			ge::Texture2D* tex, int breakableWallRandomnessIndex);
	}
}