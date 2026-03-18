#pragma once
#include "GameObjectCommand.h"
#include <memory>
#include <functional>

namespace ge
{
	class GameObject;

	// Engine Command Wrapper executing command if condition is met
	class ConditionalCommand final : public GameObjectCommand
	{
	public:
		ConditionalCommand(std::unique_ptr<GameObjectCommand> wrappedCommand, std::function<bool()> condition);

		virtual void Execute(float deltaTime) override;

	private:
		std::unique_ptr<GameObjectCommand> m_WrappedCommand{};
		std::function<bool()> m_Condition{};
	};
}