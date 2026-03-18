#include "ConditionalCommand.h"

using namespace ge;

ConditionalCommand::ConditionalCommand(std::unique_ptr<GameObjectCommand> wrappedCommand, std::function<bool()> condition)
	: GameObjectCommand::GameObjectCommand(wrappedCommand->GetCommandTarget()),
	m_WrappedCommand{ std::move(wrappedCommand) },
	m_Condition{ std::move(condition) }
{
}

void ge::ConditionalCommand::Execute(float deltaTime)
{
	if (m_Condition())
	{
		m_WrappedCommand->Execute(deltaTime);
	}
}
