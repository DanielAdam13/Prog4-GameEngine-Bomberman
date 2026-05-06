#pragma once
#include "Components/Component.h"

#include "ObservableSubject.h"

namespace bombGame
{
	class BombComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Health Display Component Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 11 };

		BombComponent(ge::GameObject* owner, float explosionTimer);
		~BombComponent() override = default;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override;
		void RenderComponent() const override;

		float GetWindupDuration() const noexcept { return m_WindUpDuration; };
		ge::Subject& GetExplodedBombEvent() { return m_ExplodedBombEvent; }

	private:
		float m_WindUpDuration;
		float m_ExplosionTimer;

		ge::Subject m_ExplodedBombEvent;
	};
}