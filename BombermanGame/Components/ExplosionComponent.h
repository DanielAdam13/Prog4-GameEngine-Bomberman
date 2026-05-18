#pragma once
#include "Components/Component.h"

namespace ge
{
	class Texture2D;
}

namespace bombGame
{
	// Simply deletes the game object after a duration, that's all this component does
	class ExplosionComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Explosion Component Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 16 };

		ExplosionComponent(ge::GameObject* owner, float explosionDuration);
		~ExplosionComponent() override = default;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override;
		void RenderComponent() const override;

		float GetExplosionDuration() const noexcept { return m_ExplosionDuration; };

	private:
		float m_ExplosionDuration;
		float m_ExplosionTimer{ 0.f };
	};
}