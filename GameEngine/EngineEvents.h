namespace ge
{
	enum EngineEventId
	{
		HEALTH_TAKING_DAMAGE = 10000,  // offset so it doesn't interfere with Game events...
		HEALTH_DIED,
		SCORE_CHANGED,
		COLLISION_ENTER, 
		COLLISION_EXIT,
		WINDOW_RESIZED,
		SCENE_CHANGED,
		ANIMATION_FINISHED
	};
}