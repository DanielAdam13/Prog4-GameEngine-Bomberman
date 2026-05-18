#pragma once
#include "Components/Component.h"
#include "Observer.h"
#include <ObservableSubject.h>

#include <functional>

namespace ge
{
    class Texture2D;
}

namespace bombGame
{
    class LevelGrid;

    // Keeps track of active bombs, increments when a bomb is Registered, decrements by listening to BOMB_EXPLODE
    class BombLayerComponent final : public ge::Component, public ge::IObserver
    {
    public:
        // ---- TYPE IDENTIFIER ----
        // Every Bomb Layer Component Instance shares the same component type ID
        static constexpr ge::ComponentTypeID StaticTypeID{ 12 };

        BombLayerComponent(ge::GameObject* owner, LevelGrid* levelBuilder,
            ge::Texture2D* textureRef, ge::Texture2D* explosionTextureRef,
            std::function<float()> windupDurationFn, int maxBombs = 1);
        ~BombLayerComponent() override = default;

        BombLayerComponent(const BombLayerComponent&) = delete;
        BombLayerComponent(BombLayerComponent&&) = delete;
        BombLayerComponent& operator=(const BombLayerComponent&) = delete;
        BombLayerComponent& operator=(BombLayerComponent&&) = delete;

        void FixedUpdateComponent(float) override {}
        void UpdateComponent(float) override {}
        void RenderComponent() const override {}

        bool TryLayBomb(const glm::vec3& position);

        int GetActiveBombs() const noexcept;
        int GetMaxBombs() const noexcept;
        void SetMaxBombs(int newMaxBombNr) noexcept;

        // Called when one of the bombs explode
        void Notify(int eventId, ge::GameObject* source) override;

        ge::Subject& GetLaidBombEvent() noexcept;
        ge::Subject& GetBombExplodedEvent() noexcept;

    private:
        int m_MaxBombs;
        int m_ActiveBombs{ 0 };
        ge::Texture2D* m_BombTextureRef;
        ge::Texture2D* m_ExplosionTextureRef;
        std::function<float()> m_WindupDurationFn;
        LevelGrid* m_LevelGridRef; // Cached ref


        ge::Subject m_LaidBombEvent;
        ge::Subject m_BombExplodedEvent; // Per Component notify, not per bomb

        // Private helper
        bool CanLayBomb() const noexcept;

        // Subscribes to the bomb's explosion event
        void RegisterLaidBomb(ge::GameObject* bomb);
    };
}