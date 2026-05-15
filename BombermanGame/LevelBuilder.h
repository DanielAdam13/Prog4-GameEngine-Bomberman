#pragma once
#include "LevelLoader.h"
#include "Structs.h"

#include <optional>

namespace ge
{
	class Scene;
	class Texture2D;
}

namespace bombGame
{
	struct GridTile
	{
		ge::structs::Rect gridRect;
		int gridTileIndex;
		levelLoader::TileType gridTileType;
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
		std::optional<glm::vec2> GetMiddlePointGridAt(const glm::vec3& worldPos) const noexcept;

		glm::vec3 GetLevelTopLeft() const noexcept;
		levelLoader::LevelLayout GetLevelLayout() const noexcept;
		float GetTileSize() const noexcept;

	private:
		levelLoader::LevelLayout m_LevelLayout;
		glm::vec3 m_LevelTopLeftPos;
		float m_TileSize;
	};

	namespace levelBuilder
	{
		void BuildStaticGeometry(ge::Scene& scene, const LevelGrid& grid);
		void GenerateDynamicObjects(ge::Scene& scene, const LevelGrid& grid,
			ge::Texture2D* tex, int breakableWallRandomnessIndex);
	}
}