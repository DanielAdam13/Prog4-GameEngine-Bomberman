#pragma once

namespace bombGame
{
	enum EventId
	{
		PLAYER_DIED,
		GAME_OVER
	};


	class Event final
	{
	public:
		explicit Event(EventId id);

	private:
		const EventId m_Id;
	};
}