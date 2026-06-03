#pragma once
#include "LevelGrid.h"

namespace ge
{
	class Scene;
	class SpriteSheet;
	class Texture2D;
}

namespace bombGame
{
	namespace levelBuilder
	{
		void BuildStaticGeometry(ge::Scene& scene, const LevelGrid& grid);
		// Generate and push the dynamic walls to the scene and store them in the grid as a reference
		void GenerateDynamicObjects(ge::Scene& scene, LevelGrid& grid,
			ge::SpriteSheet* breakableWallSheet, ge::Texture2D* exitDoorTexture,
			int breakableWallRandomnessIndex);
	}
}