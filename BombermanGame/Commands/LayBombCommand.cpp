#include "LayBombCommand.h"
#include "GameObject.h"
#include "SceneManager.h"

#include <Components/Transform.h>
#include "Components/BombComponent.h"
#include "Components/BombLayerComponent.h"
#include <Texture2D.h>

#include "GameEvents.h"

#include <memory>
#include <utility>

#include "Utils.h"


bombGame::LayBombCommand::LayBombCommand(ge::GameObject* commandTarget, ge::Texture2D* textureRef, std::function<float()> explosionTimerFn)
	:ge::GameObjectCommand::GameObjectCommand(commandTarget),
	m_BombTextureRef{ textureRef },
	m_ExplosionTimerFunction{ explosionTimerFn },
	m_LayedBombEvent{}
{
}

void bombGame::LayBombCommand::Execute(float)
{	
	auto* bombLayerComp{ GetCommandTarget()->GetComponent<BombLayerComponent>() };

	// Can Lay bomb check
	if (!bombLayerComp || !bombLayerComp->CanLayBomb())
		return;

	ge::Transform* targetTransform{ GetCommandTarget()->GetComponent<ge::Transform>() };
	const glm::vec3 pos{ targetTransform->GetWorldPosition() };

	std::unique_ptr<ge::GameObject> bomb{ CreateBomb(pos, m_BombTextureRef, m_ExplosionTimerFunction()) };
	auto* bombRaw{ bomb.get() };

	ge::SceneManager::GetInstance().GetCurrentScene()->Add(std::move(bomb));

	// !!!!! Register bomb to BOMB LAYER, else there is no active bomb check !!!!!!
	bombLayerComp->RegisterLaidBomb(bombRaw);
	m_LayedBombEvent.NotifyObservers(EventId::LAY_BOMB, GetCommandTarget());
}
