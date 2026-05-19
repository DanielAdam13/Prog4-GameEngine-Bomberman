#pragma once

#include <string>
#include <vector>

namespace ge
{
	// Just a struct holding a container of indices sampled from a spriteSheet.
	struct Animation final
	{
		std::string name;
		std::vector<int> frames;
		float framesPerSecond;
		bool loops;
	};
}
