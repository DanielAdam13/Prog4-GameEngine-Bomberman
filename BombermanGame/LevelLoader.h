#pragma once

#include <vector>
#include <string>
#include <utility>

namespace bombGame
{
	namespace levelLoader
	{
		enum class TileType
		{
			Empty,
			Wall,
			PlayerSpawn
		};

		// Stores the level data read from the FILE for the level (intial static data)
		struct LevelLayout
		{
			int width;
			int height;
			std::vector<TileType> tilesContainer;
			std::pair<int, int> player1SpawnPoint;
			std::pair<int, int> player2SpawnPoint;

			// Helper that returns TileType by row/col
			TileType At(int col, int row) const
			{
				return tilesContainer[row * width + col];
			}
		};

		LevelLayout Load(const std::string& path);
	}
}