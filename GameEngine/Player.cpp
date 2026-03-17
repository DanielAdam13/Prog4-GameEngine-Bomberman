#include "Player.h"
#include "GameObject.h"
#include "Components/Image.h"
#include "Components/Transform.h"

using namespace bombGame;

Player::Player(ge::GameObject* playerObject, ge::Texture2D* playerTexture)
	:m_pPlayerObject{ playerObject },
	m_pPlayerTexture{ playerTexture },
	m_pPlayerTransform{ m_pPlayerObject->GetComponent<ge::Transform>() }
{
	m_pPlayerObject->AddComponent<ge::Image>(m_pPlayerObject)->SetTexture(playerTexture);
}

void Player::SetPlayerPosition(const glm::vec3& newPos)
{
	m_pPlayerTransform->SetLocalPosition(m_pPlayerTransform->GetWorldPosition() + newPos);
}

void Player::SetPlayerScale(const glm::vec3& newScale)
{
	m_pPlayerTransform->SetLocalScale(newScale);
}
