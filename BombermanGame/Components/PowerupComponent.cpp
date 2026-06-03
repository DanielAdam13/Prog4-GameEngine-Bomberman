#include "PowerupComponent.h"

#include "GameObject.h"
#include "BombLayerComponent.h"
#include "PlayerComponent.h"

bombGame::PowerupComponent::PowerupComponent(ge::GameObject* owner, PowerupType type, int score)
	:Component::Component(owner),
	m_Type{ type },
	m_PickupScore{ score }
{
}

void bombGame::PowerupComponent::ApplyTo(ge::GameObject* player)
{
	switch (m_Type)
	{
	case bombGame::BombUp:
		if (auto* layerComp = player->GetComponent<BombLayerComponent>())
		{
			layerComp->SetMaxBombs(layerComp->GetMaxBombs() + 1);
		}
		break;
	case bombGame::FireUp:
		if (auto* layerComp = player->GetComponent<BombLayerComponent>())
		{
			layerComp->SetExplosionArmLength(layerComp->GetExplosionArmLength() + 1);
		}
		break;
	case bombGame::RemoteDetonate:
		break;
	}

	// Delete
	GetOwner()->MarkForDeletion();
}
