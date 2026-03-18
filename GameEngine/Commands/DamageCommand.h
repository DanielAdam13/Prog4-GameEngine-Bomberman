#pragma once
#include "GameObjectCommand.h"

namespace ge
{
	class GameObject;
	class HealthComponent;
}

namespace bombGame
{
	// Game-specific Command
	class DamageCommand final : public ge::GameObjectCommand
	{
	public:
		DamageCommand(ge::GameObject* commandTarget, int damageAmount = 1);

		virtual void Execute(float) override;

		void ChangeDamageAmount(int newDamage);

	private:
		int m_DamageAmount;
	};
}