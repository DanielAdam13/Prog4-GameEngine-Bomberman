#pragma once
#include "Components/Component.h"
#include "Observer.h"
#include <ObservableSubject.h>

#include <functional>
#include <array>

namespace ge
{
    class SpriteSheet;
}

namespace bombGame
{
    class LevelGrid;

    // Keeps track of active bombs, increments when a bomb is Registered, decrements by listening to BOMB_EXPLODE from Bomb GOs.
    class BombLayerComponent final : public ge::Component, public ge::IObserver
    {
    public:
        // ---- TYPE IDENTIFIER ----
        // Every Bomb Layer Component Instance shares the same component type ID
        static constexpr ge::ComponentTypeID StaticTypeID{ 12 };

        BombLayerComponent(ge::GameObject* owner, LevelGrid* levelBuilder,
            ge::SpriteSheet* bombSheetRef, std::array<ge::SpriteSheet*, 3> explosionSheetsRef,
            float startingWindup = 3.f, int startingExplArm = 1, int maxBombs = 1);
        ~BombLayerComponent() override = default;

        BombLayerComponent(const BombLayerComponent&) = delete;
        BombLayerComponent(BombLayerComponent&&) = delete;
        BombLayerComponent& operator=(const BombLayerComponent&) = delete;
        BombLayerComponent& operator=(BombLayerComponent&&) = delete;

        void FixedUpdateComponent(float) override {}
        void UpdateComponent(float) override {}
        void RenderComponent() const override {}

        bool TryLayBomb(const glm::vec3& position);

        int GetActiveBombs() const noexcept { return m_ActiveBombs; }
        int GetMaxBombs() const noexcept { return m_MaxBombs; }
        void SetMaxBombs(int newMaxBombNr) noexcept;
        int GetExplosionArmLength() const noexcept { return m_ExplosionArmLength; }
        void SetExplosionArmLength(int newLength);

        // Called when one of the bombs explode
        void Notify(int eventId, ge::GameObject* source) override;

        ge::Subject& GetLaidBombEvent() noexcept;
        ge::Subject& GetBombExplodedEvent() noexcept;

    private:
        int m_MaxBombs;
        int m_ActiveBombs{ 0 };
        ge::SpriteSheet* m_BombSheetRef;
        std::array<ge::SpriteSheet*, 3> m_ExplosionSheetsRef;
        float m_WindupDuration;
        int m_ExplosionArmLength;

        LevelGrid* m_LevelGridRef; // Cached ref

        // If an observer is interested of the action that a bomb has exploded -> subscribe to this bomb exploded event, 
        // not the Bomb GO specific one
        ge::Subject m_LaidBombEvent;
        ge::Subject m_BombExplodedEvent; // Per Component notify, not per bomb

        // Private helper
        bool CanLayBomb() const noexcept;

        // Subscribes to the bomb's explosion event
        void RegisterLaidBomb(ge::GameObject* bomb);
    };
}