#pragma once
#include "Command.h"

namespace ge
{
	class GameObject;

	class GameObjectCommand : public Command
	{
	public:
		GameObjectCommand(GameObject* target)
			: Command::Command(),
			m_Target{ target }
		{
		};

		virtual ~GameObjectCommand() override = default;

		virtual void Execute(float) override = 0;

		// Public for unbinding per target
		GameObject* GetCommandTarget() const noexcept { return m_Target; } 

	protected:

	private:
		GameObject* m_Target;

	};
}