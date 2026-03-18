#include "Player.h"
#include <cassert>

#include "GameObject.h"
#include "Components/Image.h"
#include "Components/Transform.h"
#include "ObservableSubject.h"
#include "Achievements.h"

using namespace bombGame;

Player::Player(ge::GameObject* playerObject, ge::Texture2D* playerTexture,
	float startSpeed, const glm::vec3& startPos, const glm::vec3& startScale)
	:m_pPlayerObject{ playerObject },
	m_pPlayerTexture{ playerTexture },
	m_pPlayerTransform{ m_pPlayerObject->GetComponent<ge::Transform>() },
	m_Speed{ startSpeed },
	m_PlayerDeadEvent{ std::make_unique<Subject>(EventId::PLAYER_DIED) }
{
	if(playerTexture)
		m_pPlayerObject->AddComponent<ge::Image>(m_pPlayerObject)->SetTexture(playerTexture);
	else
		assert("No player texture assigned");

	m_pPlayerTransform->SetLocalPosition(m_pPlayerTransform->GetWorldPosition() + startPos);
	m_pPlayerTransform->SetLocalScale(startScale);
}

Player::~Player()
{
}

void Player::SetPlayerPosition(const glm::vec3& newPos)
{
	m_pPlayerTransform->SetLocalPosition(m_pPlayerTransform->GetWorldPosition() + newPos);
}

void Player::SetPlayerScale(const glm::vec3& newScale)
{
	m_pPlayerTransform->SetLocalScale(newScale);
}

glm::vec3 Player::GetPlayerPosition() const noexcept
{
	return m_pPlayerTransform->GetWorldPosition();
}

glm::vec3 Player::GetPlayerScale() const noexcept
{
	return m_pPlayerTransform->GetWorldScale();
}

float Player::GetSpeed() const noexcept
{
	return m_Speed;
}

void Player::SetSpeed(float newSpeed) noexcept
{
	m_Speed = newSpeed;
}

Subject& bombGame::Player::GetDeadEvent() const
{
	return *m_PlayerDeadEvent;
}
