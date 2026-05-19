#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <functional>

namespace ge
{
	class GameObject;
	class Texture2D;
	class SpriteSheet;
}

namespace bombGame
{
	namespace spawnUtils
	{
		// Creates the reusable Bomb Game Object and returns it as a unique_ptr
		std::unique_ptr<ge::GameObject> CreateBomb(const glm::vec3& position, 
			ge::SpriteSheet* bombTexture, ge::SpriteSheet* explosionTextureRef, float windupTimer);

		// Creates the Explosion Game Object on the correct collision Layer
		std::unique_ptr<ge::GameObject> CreateExplosion(const glm::vec3& fixedPosition, ge::SpriteSheet* texture, float activeTimer);
	}
}