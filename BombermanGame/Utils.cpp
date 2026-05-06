#include "Utils.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Image.h"
#include "Components/BombComponent.h"

#include "BombermanGame.h"
#include "SoundManager.h"

std::unique_ptr<ge::GameObject> bombGame::CreateBomb(const glm::vec3& position, ge::Texture2D* texture, float explosionTimer)
{
	auto bomb = std::make_unique<ge::GameObject>("GO_Bomb");
	bomb->GetComponent<ge::Transform>()->SetLocalPosition(position);
	bomb->AddComponent<ge::Image>(bomb.get())->SetTexture(texture);

	BombComponent* bombComp{ bomb->AddComponent<BombComponent>(bomb.get(), explosionTimer) };

	// Wire observers per-instance — each bomb has its own subjects
	bombComp->GetExplodedBombEvent().AddObserver(&BombermanGame::GetSoundManager());

	return bomb;
}
