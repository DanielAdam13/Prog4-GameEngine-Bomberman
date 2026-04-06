#pragma once
#include "Component.h"
#include "Observer.h"

namespace ge
{
	class TextComponent;
}

namespace bombGame
{
	class ScoreDisplayComponent final : public ge::Component, public ge::IObserver
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Score Display Component Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 9 };

		ScoreDisplayComponent(ge::GameObject* owner, ge::GameObject* trackedPlayer);
		~ScoreDisplayComponent() override;

		virtual void Notify(int eventId, ge::GameObject*) override;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override {}
		void RenderComponent() const override {}

	private:
		ge::GameObject* m_pTrackedPlayer; // non-owning
		ge::TextComponent* m_pCachedText; // non-owning

		void RefreshText();
	};
}