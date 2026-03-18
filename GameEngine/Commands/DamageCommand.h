#pragma once
#include "GameObjectCommand.h"

namespace ge
{
	class GameObject;
	class HealthComponent;

	class DamageCommand final : public GameObjectCommand
	{
	public:
		DamageCommand(GameObject* target, int damageAmount = 1);

		virtual void Execute(float) override;

		void ChangeDamageAmount(int newDamage);

	private:
		int m_DamageAmount;
	};
}