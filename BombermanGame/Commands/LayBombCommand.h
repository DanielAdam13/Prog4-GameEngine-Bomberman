#pragma once
#include "Commands/GameObjectCommand.h"
#include "ObservableSubject.h"

#include <functional>

namespace ge
{
	class GameObject;
	class Texture2D;
}

namespace bombGame
{
	// Game-specific Command, is a Subject for the Lay_Bomb event
	class LayBombCommand final : public ge::GameObjectCommand
	{
	public:
		LayBombCommand(ge::GameObject* commandTarget, ge::Texture2D* textureRef, std::function<float()> explosionTimerFn);

		virtual void Execute(float) override;

		ge::Subject& GetLayedBombEvent() { return m_LayedBombEvent; }

	private:
		ge::Texture2D* m_BombTextureRef;
		std::function<float()> m_ExplosionTimerFunction;

		ge::Subject m_LayedBombEvent;
	};
}