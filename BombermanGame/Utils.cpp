#include "Utils.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Image.h"
#include "Components/AnimatorComponent.h"
#include "Components/BombComponent.h"
#include "Components/ExplosionComponent.h"

#include "Animation.h"

std::unique_ptr<ge::GameObject> bombGame::spawnUtils::CreateBomb(const glm::vec3& position, 
	ge::SpriteSheet* bombSheet, ge::SpriteSheet* explosionSheet, float explosionTimer)
{
	auto bomb = std::make_unique<ge::GameObject>("GO_Bomb");
	auto* bombTr{ bomb->GetComponent<ge::Transform>() };
	bombTr->SetLocalPosition(position);
	bombTr->SetLocalScale(2.f, 2.f, 1.f);
	
	auto* animator{ bomb->AddComponent<ge::AnimatorComponent>(bomb.get(), bombSheet) };
	animator->SetAnchor({ 0.5f, 0.5f });
	animator->AddAnimation({ "wind_up", {0, 1, 2}, 4, true });
	animator->Play("wind_up");

	bomb->AddComponent<BombComponent>(bomb.get(), explosionTimer, explosionSheet);

	return bomb;
}

std::unique_ptr<ge::GameObject> bombGame::spawnUtils::CreateExplosion(const glm::vec3& fixedPosition, 
	ge::SpriteSheet* explosionSheet, float activeTimer)
{
	auto explosion = std::make_unique<ge::GameObject>("GO_Explosion");
	auto* explosionTr{ explosion->GetComponent<ge::Transform>() };
	explosionTr->SetLocalPosition(fixedPosition);
	explosionTr->SetLocalScale(2.f, 2.f, 1.f);

	auto* animator{ explosion->AddComponent<ge::AnimatorComponent>(explosion.get(), explosionSheet)};
	animator->SetAnchor({ 0.5f, 0.5f });
	animator->AddAnimation({ "explode", {0, 1, 2, 3}, 5, false });
	animator->Play("explode");

	explosion->AddComponent<ExplosionComponent>(explosion.get(), activeTimer);

	return explosion;
}
