#pragma once
#include "Components/Component.h"

namespace ge
{
	class AnimatorComponent;
}

namespace bombGame
{
	// Simply deletes the game object after a duration. However, this is done only when Crumble() is called.
	class BreakableWallComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Explosion Component Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 9 };

		BreakableWallComponent(ge::GameObject* owner, float crumblingDuration);
		~BreakableWallComponent() override = default;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override;
		void RenderComponent() const override {}

		void Crumble() noexcept;

		float GetCrumblingDuration() const noexcept { return m_CrumblingDuration; };

	private:
		float m_CrumblingDuration;
		float m_CrumblingTimer{ 0.f };

		ge::AnimatorComponent* m_CachedAnimatorComp;
		bool m_CrumbleFlag{ false };
	};
}