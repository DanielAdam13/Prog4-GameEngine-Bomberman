#pragma once

#include <glm/glm.hpp>

namespace ge
{
	namespace structs
	{
		struct Rect
		{
			Rect(const glm::vec2& pos, const glm::vec2& size)
				:position{ pos },
				size{ size }
			{
			}

			Rect(float x, float y, float w, float h)
				:Rect::Rect({ x,y }, { w,h })
			{
			}

			glm::vec2 position;
			glm::vec2 size;
		};

		struct Circle
		{
			Circle(const glm::vec2& center, float rad)
				:centerPos{ center },
				radius{ rad }
			{
			}

			glm::vec2 centerPos;
			float radius;
		};
	}
}