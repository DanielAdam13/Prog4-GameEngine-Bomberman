#pragma once
#include <glm/glm.hpp>
#include <memory>

namespace ge
{
	class GameObject;
	class Texture2D;

	class Transform;
	class HealthComponent;
}

namespace bombGame
{
	class Subject;

	// Temporary implemetation before splitting GameEngine into a library
	class Player final
	{
	public:
		Player(ge::GameObject* playerObject, ge::Texture2D* playerTexture, 
			float speed = 120.f, int playerHp = 3, const glm::vec3& startPos = {}, const glm::vec3& startScale = {1.f, 1.f, 1.f});
		~Player();

		void SetPlayerTexture(ge::Texture2D* newTexture);

		void SetPlayerPosition(const glm::vec3& newPos);
		void SetPlayerScale(const glm::vec3& newScale);

		glm::vec3 GetPlayerPosition() const noexcept;
		glm::vec3 GetPlayerScale() const noexcept;

		float GetPlayerSpeed() const noexcept;
		void SetPlayerSpeed(float newSpeed) noexcept;

		Subject& GetDeadEvent() const;

	private:
		// Just references, doesn't own:
		ge::GameObject* m_pPlayerObject;
		ge::Transform* m_CachedPlayerTransform;
		ge::HealthComponent* m_CachedPlayerHealth;

		float m_Speed;

		// Event lives on player, observer is higher (scene/game)
		std::unique_ptr<Subject> m_PlayerDeadEvent; 
	};
}
