#pragma once
#include "Components/Component.h"
#include "PowerupType.h"

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	// All this component does is store the type and score for a powerup GO
	class PowerupComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every PowerupComponent Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 20 };

		PowerupComponent(ge::GameObject* owner, PowerupType type, int score);
		~PowerupComponent() override = default;

		void FixedUpdateComponent(float) override {} // nothing
		void UpdateComponent(float) override {} // nothing
		void RenderComponent() const override {} // nothing

		// Old actual powerupEntry LOGIC
		/*void ApplyTo(ge::GameObject* player);*/

		// Only getters, set from creation via constructor:
		PowerupType GetType() const noexcept { return m_Type; }
		int GetPickupScore() const noexcept { return m_PickupScore; }

	private:
		PowerupType m_Type;
		int m_PickupScore;

	};
}