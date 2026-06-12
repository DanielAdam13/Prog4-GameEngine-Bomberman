#pragma once
#include "GameState.h"

#include <memory>

namespace bombGame
{
    class BombermanGame;

    class HighScoreState : public GameState 
    {
    public:
        HighScoreState(BombermanGame& game);

        virtual void OnEnter() override;
        virtual void OnExit() override;

        virtual std::unique_ptr<GameState> Update(float) override { return nullptr; }
        virtual void FixedUpdate(float) override {}

    private:
        void BindNavigationInput();

    };
}