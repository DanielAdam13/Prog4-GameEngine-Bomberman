#include "PowerupEffects.h"
#include "GameObject.h"
#include "Components/PlayerComponent.h"
#include "Components/BombLayerComponent.h"
#include "BombermanGame.h"

void bombGame::powerupEffects::OnPickup(PowerupType type, const std::vector<ge::GameObject*>& players)
{
	for (auto* p : players)
	{
		if (!p || p->MarkedForDeletion())
			continue;

		auto* playerComp{ p->GetComponent<bombGame::PlayerComponent>() };

		// Player needs to be alive
		if (!playerComp || !playerComp->IsAlive())
			continue;

		ApplyEffectToPlayer(p, type);
	}
}

void bombGame::powerupEffects::ApplyEffectToPlayer(ge::GameObject* player, PowerupType type)
{
	auto* layerComp{ player->GetComponent<BombLayerComponent>() };

	if (!layerComp)
		return;

	switch (type)
	{
	case bombGame::BombUp:
		layerComp->SetMaxBombs(layerComp->GetMaxBombs() + 1);
		break;
	case bombGame::FireUp:
		layerComp->SetExplosionArmLength(layerComp->GetExplosionArmLength() + 1);
		break;
	case bombGame::RemoteDetonate:
		layerComp->EnableRemoteDetonation();
		break;
	}
}

void bombGame::powerupEffects::ApplyStoredToPlayer(ge::GameObject* player, const std::vector<PowerupType>& stored)
{
	for (auto type : stored)
	{
		if (!player || player->MarkedForDeletion())
			return;

		ApplyEffectToPlayer(player, type);
	}
}
