#include "DamageCommand.h"
#include "GameObject.h"
#include "Components/HealthComponent.h"

using namespace bombGame;

DamageCommand::DamageCommand(ge::GameObject* commandTarget, int damageAmount)
	:ge::GameObjectCommand::GameObjectCommand(commandTarget),
	m_DamageAmount{ damageAmount }
{
}

void DamageCommand::Execute(float)
{
	auto* targetHealthComponent{ GetCommandTarget()->GetComponent<HealthComponent>() };

	if (targetHealthComponent)
		targetHealthComponent->TakeDamage(m_DamageAmount);

}

void DamageCommand::ChangeDamageAmount(int newDamage)
{
	m_DamageAmount = newDamage;
}
