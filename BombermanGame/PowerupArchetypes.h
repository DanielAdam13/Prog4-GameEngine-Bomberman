#pragma once
#include "PowerupType.h"

namespace ge
{
	class Texture2D;
}

namespace bombGame
{
	struct PowerupArchetype
	{
		ge::Texture2D* texture{ nullptr };
		int scoreValuePicked{ 200 };
	};

	namespace powerupArchetypes
	{
		const PowerupArchetype& Get(PowerupType type);
		void InitializeArchetypes(ge::Texture2D* fireUpTexture,
			ge::Texture2D* bombUpTexture,
			ge::Texture2D* remoteDetonateTexture);
	}
}