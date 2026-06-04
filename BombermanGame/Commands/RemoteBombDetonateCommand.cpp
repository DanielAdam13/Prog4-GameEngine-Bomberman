#include "RemoteBombDetonateCommand.h"
#include "GameObject.h"
#include "Components/BombLayerComponent.h"

bombGame::RemoteBombDetonateCommand::RemoteBombDetonateCommand(ge::GameObject* commandTarget)
	:GameObjectCommand::GameObjectCommand(commandTarget)
{
}

void bombGame::RemoteBombDetonateCommand::Execute(float)
{
	auto* bombLayerComp{ GetCommandTarget()->GetComponent<BombLayerComponent>() };
	if (!bombLayerComp)
		return;

	bombLayerComp->DetonateAllBombsIfPossible();
}
