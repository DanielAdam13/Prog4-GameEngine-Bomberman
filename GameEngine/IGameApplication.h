#pragma once

namespace ge
{
	// Pure virtual interface. Acts as the base for every Game
	// Simply allows Load, Update and FixedUpdate overloads
	class IGameApplication
	{
	public:
		virtual ~IGameApplication() = default;
		virtual void Load() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void FixedUpdate(float fixedDelta) = 0;
	};
}