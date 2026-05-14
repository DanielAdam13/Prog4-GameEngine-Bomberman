#include "Utils.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Image.h"
#include "Components/BombComponent.h"

std::unique_ptr<ge::GameObject> bombGame::CreateBomb(const glm::vec3& position, ge::Texture2D* texture, float explosionTimer)
{
	auto bomb = std::make_unique<ge::GameObject>("GO_Bomb");
	auto bombTr{ bomb->GetComponent<ge::Transform>() };
	bombTr->SetLocalPosition(position);
	bombTr->SetLocalScale(2.f, 2.f, 1.f);
	
	bomb->AddComponent<ge::Image>(bomb.get())->SetTexture(texture);

	bomb->AddComponent<BombComponent>(bomb.get(), explosionTimer);

	return bomb;
}
