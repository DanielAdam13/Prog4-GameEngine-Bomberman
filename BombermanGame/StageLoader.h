#pragma once
#include "EnemyType.h"
#include "PowerupType.h"

#include <vector>
#include <string>

namespace bombGame
{
	namespace stageLoader
	{
		struct EnemyEntry
		{
			EnemyType type;
			int count;
		};

		struct StageDescriptor
		{
			std::string layoutFile;
			int breakableDensity;
			std::vector<EnemyEntry> enemies;
			PowerupType powerupType;
		};

		const StageDescriptor& Load(size_t stageIndex);
		StageDescriptor Load(const std::string& filePath);

		int GetStageCount() noexcept;
	}
}