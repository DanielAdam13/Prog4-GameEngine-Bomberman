#include "Player.h"
#include <cassert>

#include "GameObject.h"
#include "Components/Image.h"
#include "Components/Transform.h"
#include "Components/HealthComponent.h"

#include "ObservableSubject.h"
#include "Achievements.h"

using namespace bombGame;

Player::Player(ge::GameObject* playerObject, ge::Texture2D* playerTexture,
	float startSpeed, int playerHp, const glm::vec3& startPos, const glm::vec3& startScale)
	:m_pPlayerObject{ playerObject },
	m_CachedPlayerTransform{ m_pPlayerObject->GetComponent<ge::Transform>() },
	m_Speed{ startSpeed },
	m_PlayerDeadEvent{ std::make_unique<Subject>(EventId::PLAYER_DIED) }
{
	if(playerTexture)
		m_pPlayerObject->AddComponent<ge::Image>(m_pPlayerObject)->SetTexture(playerTexture);
	else
		assert("No player texture assigned");

	m_CachedPlayerTransform->SetLocalPosition(m_CachedPlayerTransform->GetWorldPosition() + startPos);
	m_CachedPlayerTransform->SetLocalScale(startScale);

	m_CachedPlayerHealth = m_pPlayerObject->AddComponent<ge::HealthComponent>(m_pPlayerObject, playerHp);
	m_CachedPlayerHealth->SetOnDeathCallback([this]()
		{
			m_PlayerDeadEvent->NotifyObservers(m_pPlayerObject);
		});
}

Player::~Player()
{
}

void Player::SetPlayerTexture(ge::Texture2D* newTexture)
{
	// Don't delete old texture since it could be reused, depends on the user in Main.cpp / Game
	if (newTexture)
		m_pPlayerObject->GetComponent<ge::Image>()->SetTexture(newTexture);
}

void Player::SetPlayerPosition(const glm::vec3& newPos)
{
	m_CachedPlayerTransform->SetLocalPosition(m_CachedPlayerTransform->GetWorldPosition() + newPos);
}

void Player::SetPlayerScale(const glm::vec3& newScale)
{
	m_CachedPlayerTransform->SetLocalScale(newScale);
}

glm::vec3 Player::GetPlayerPosition() const noexcept
{
	return m_CachedPlayerTransform->GetWorldPosition();
}

glm::vec3 Player::GetPlayerScale() const noexcept
{
	return m_CachedPlayerTransform->GetWorldScale();
}

float Player::GetPlayerSpeed() const noexcept
{
	return m_Speed;
}

void Player::SetPlayerSpeed(float newSpeed) noexcept
{
	m_Speed = newSpeed;
}

Subject& bombGame::Player::GetDeadEvent() const
{
	return *m_PlayerDeadEvent;
}
