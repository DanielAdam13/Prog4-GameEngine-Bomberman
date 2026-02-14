#pragma once
#include "Singleton.h"

namespace ge
{
	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();
	};

}
