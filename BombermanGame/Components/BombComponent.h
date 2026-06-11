#pragma once
#include "Components/Component.h"
#include "ObservableSubject.h"

#include <array>

namespace ge
{
	class SpriteSheet;
}

namespace bombGame
{
	class LevelGrid;

	// Bomb Component.
	// Has an Explosion Subject event which is used ONLY from Bomb Layer on the player GO.
	class BombComponent final : public ge::Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Bomb Component Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 11 };

		BombComponent(ge::GameObject* owner, LevelGrid& grid, 
			float windupDuration, int explosionArmLength, std::array<ge::SpriteSheet*, 3>& explosionSheetRef);
		~BombComponent() override = default;

		void FixedUpdateComponent(float) override {}
		void UpdateComponent(float) override;
		void RenderComponent() const override {}

		float GetWindupDuration() const noexcept { return m_WindUpDuration; };
		ge::Subject& GetExplodedBombEvent() { return m_ExplodedBombEvent; }

		void ForceDetonate() noexcept;

	private:
		float m_WindUpDuration;
		float m_WindupTimer;

		// If anything else subscribes to this event, it means that they would need information about THE BOMB Game Object itself.
		ge::Subject m_ExplodedBombEvent;

		// --- Explosion data ---
		std::array<ge::SpriteSheet*, 3> m_ExplosionSheetsRef; // Cached references
		LevelGrid& m_CachedGrid; // Cached ref
		static constexpr float m_ExplosionLifetime{ 0.6f };
		const int m_ExplosionArmLength;
	};
}