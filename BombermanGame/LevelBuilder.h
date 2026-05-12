#pragma once
#include "LevelLoader.h"

namespace ge
{
	class Scene;
}

namespace bombGame
{
	namespace levelBuilder
	{
		// Going to change scene by adding game objects and READ layout
		void BuildStaticGeometry(ge::Scene& scene, const levelLoader::LevelLayout& layout,
			const glm::vec3& buildTopLeftPos, const float tileSize);
	}
}