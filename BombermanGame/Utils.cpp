#include "Utils.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Image.h"
#include "Components/BombComponent.h"
#include "Components/ExplosionComponent.h"

std::unique_ptr<ge::GameObject> bombGame::spawnUtils::CreateBomb(const glm::vec3& position, 
	ge::Texture2D* bombTexture, ge::Texture2D* explosionTextureRef, float explosionTimer)
{
	auto bomb = std::make_unique<ge::GameObject>("GO_Bomb");
	auto* bombTr{ bomb->GetComponent<ge::Transform>() };
	bombTr->SetLocalPosition(position);
	bombTr->SetLocalScale(2.f, 2.f, 1.f);
	
	auto* img{ bomb->AddComponent<ge::Image>(bomb.get()) };
	img->SetTexture(bombTexture);
	img->SetAnchor({ 0.5f, 0.5f });

	bomb->AddComponent<BombComponent>(bomb.get(), explosionTimer, explosionTextureRef);

	return bomb;
}

std::unique_ptr<ge::GameObject> bombGame::spawnUtils::CreateExplosion(const glm::vec3& fixedPosition, ge::Texture2D* texture, float activeTimer)
{
	auto explosion = std::make_unique<ge::GameObject>("GO_Explosion");
	auto* explosionTr{ explosion->GetComponent<ge::Transform>() };
	explosionTr->SetLocalPosition(fixedPosition);
	explosionTr->SetLocalScale(2.f, 2.f, 1.f);

	auto* img{ explosion->AddComponent<ge::Image>(explosion.get()) };
	img->SetTexture(texture);
	img->SetAnchor({ 0.5f, 0.5f });

	explosion->AddComponent<ExplosionComponent>(explosion.get(), activeTimer);

	return explosion;
}
