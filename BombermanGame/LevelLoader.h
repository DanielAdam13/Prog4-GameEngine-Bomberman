#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>

namespace bombGame
{
	namespace levelLoader
	{
		enum TileType
		{
			Empty,
			Wall,
			PlayerSpawn
		};

		struct LevelLayout
		{
			int width;
			int height;
			std::vector<TileType> tilesContainer;
			glm::vec2 player1SpawnPoint;
			glm::vec2 player2SpawnPoint;

			// Helper that returns TileType by row/col
			TileType At(int col, int row) const
			{
				return tilesContainer[row * width + col];
			}
		};

		LevelLayout Load(const std::string& path);
	}
}