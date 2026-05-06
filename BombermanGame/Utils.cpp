#include "Utils.h"

#include "BombermanGame.h"
#include "SoundManager.h"

#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Image.h"
#include "Components/BombComponent.h"

#include "Components/EnemyComponent.h"


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

std::function<bool()> bombGame::MakePlayerInRangePredicate(ge::GameObject* enemy)
{
	return [enemy]()->bool
		{
			auto* enemyCtrl{ enemy->GetComponent<EnemyComponent>() };
			if (!enemyCtrl || enemyCtrl->GetTargets().empty())
				return false;

			const auto enemyPos{ enemy->GetComponent<ge::Transform>()->GetWorldPosition() };
			const float detRadius{ enemyCtrl->GetDetectionRadius() };

			for (auto* target : enemyCtrl->GetTargets())
			{
				const auto targetPos{ target->GetComponent<ge::Transform>()->GetWorldPosition() };

				if (glm::distance(glm::vec2{ enemyPos }, glm::vec2{ targetPos }) < detRadius)
				{
					return true;
				}
			}

			return false;
		};
}
