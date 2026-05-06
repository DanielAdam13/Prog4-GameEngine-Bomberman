#pragma once
#include "Components/Component.h"
#include "Observer.h"

namespace bombGame
{
    // Keeps track of active bombs, increments when a bomb is Registered, decrements by listening to BOMB_EXPLODE
    class BombLayerComponent final : public ge::Component, public ge::IObserver
    {
    public:
        // ---- TYPE IDENTIFIER ----
        // Every Bomb Layer Component Instance shares the same component type ID
        static constexpr ge::ComponentTypeID StaticTypeID{ 12 };

        BombLayerComponent(ge::GameObject* owner, int maxBombs = 1);
        ~BombLayerComponent() override = default;

        BombLayerComponent(const BombLayerComponent&) = delete;
        BombLayerComponent& operator=(const BombLayerComponent&) = delete;

        void FixedUpdateComponent(float) override {}
        void UpdateComponent(float) override {}
        void RenderComponent() const override {}

        bool CanLayBomb() const noexcept;
        int GetActiveBombs() const noexcept;
        int GetMaxBombs() const noexcept;
        void SetMaxBombs(int newMaxBombNr) noexcept;

        // Called by LayBombCommand when a bomb is successfully spawned
        // Subscribes to the bomb's explosion event
        void RegisterLaidBomb(ge::GameObject* bomb);

        // Called when one of the bombs explode
        void Notify(int eventId, ge::GameObject* source) override;

    private:
        int m_MaxBombs;
        int m_ActiveBombs{ 0 };
    };
}