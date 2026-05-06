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
	// Cache before predicate creation
	auto* enemyCtrl{ enemy->GetComponent<EnemyComponent>() };
	auto* enemyTf{ enemy->GetComponent<ge::Transform>() };

	return [enemyCtrl, enemyTf]()->bool
		{
			if (!enemyCtrl || enemyCtrl->GetTargets().empty())
				return false;

			const auto& targetTransforms{ enemyCtrl->GetTargetTransforms() };

			const glm::vec3 enemyPos{ enemyTf->GetWorldPosition() };
			const float radiusSqr{ enemyCtrl->GetDetectionRadius() * enemyCtrl->GetDetectionRadius() };
			
			// Check if any target is in range via Transform comp
			for (auto* targetTf : targetTransforms)
			{
				const glm::vec3 targetPos{ targetTf->GetWorldPosition() };
				const glm::vec2 diff{ targetPos - enemyPos };

				if (glm::dot(diff, diff) < radiusSqr) // squared distance
					return true;
			}

			return false;
		};
}
