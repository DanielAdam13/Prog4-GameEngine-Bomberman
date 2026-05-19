namespace ge
{
	enum EngineEventId
	{
		COLLISION_ENTER = 10000,  // offset so it doesn't interfere with Game events...
		COLLISION_EXIT,
		WINDOW_RESIZED,
		SCENE_CHANGED,
		ANIMATION_FINISHED
	};
}