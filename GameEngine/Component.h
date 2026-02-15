#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace ge
{
	// Type alias used for Unique IDs for each component TYPE
	using ComponentTypeID = uint32_t;

	class Component
	{
	public:
		Component() = default;
		virtual ~Component() = default;

		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;
	};
}