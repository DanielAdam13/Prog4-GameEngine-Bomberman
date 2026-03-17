#pragma once
#include <glm/glm.hpp>

namespace ge
{
	class GameObject;
	class Texture2D;
	class Transform;
}

namespace bombGame
{
	class Player final
	{
	public:
		Player(ge::GameObject* playerObject, ge::Texture2D* playerTexture);

		void SetPlayerPosition(const glm::vec3& newPos);
		void SetPlayerScale(const glm::vec3& newScale);

	private:
		// Just references, doesn't own:
		ge::GameObject* m_pPlayerObject; 
		ge::Texture2D* m_pPlayerTexture;
		ge::Transform* m_pPlayerTransform;

	};
}
