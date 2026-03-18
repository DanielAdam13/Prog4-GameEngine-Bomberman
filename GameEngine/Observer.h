#pragma once

namespace ge
{
	class GameObject;
}

namespace bombGame
{
	enum EventId
	{
		PLAYER_DIED,
		PLAYER_LOST_HEALTH,
		GAME_OVER
	};

	class Observer
	{
	public:
		Observer() = default;
		virtual ~Observer() = default;

		virtual void Notify(EventId event, ge::GameObject* sourceObject) = 0;

	protected:

	private:

	};
}