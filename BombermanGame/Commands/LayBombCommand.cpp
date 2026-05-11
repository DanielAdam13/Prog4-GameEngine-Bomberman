#include "LayBombCommand.h"
#include "GameObject.h"

#include <Components/Transform.h>
#include "Components/BombLayerComponent.h"
#include <Texture2D.h>

bombGame::LayBombCommand::LayBombCommand(ge::GameObject* commandTarget)
	:ge::GameObjectCommand::GameObjectCommand(commandTarget)
{
}

void bombGame::LayBombCommand::Execute(float)
{	
	auto* bombLayerComp{ GetCommandTarget()->GetComponent<BombLayerComponent>() };
	
	if (!bombLayerComp)
		return;

	ge::Transform* targetTransform{ GetCommandTarget()->GetComponent<ge::Transform>() };
	
	// Can Lay bomb check happens inside the BombLayerComponent
	bombLayerComp->TryLayBomb(targetTransform->GetWorldPosition());
}
