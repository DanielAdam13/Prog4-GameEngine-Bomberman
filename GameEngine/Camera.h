#pragma once

#include <glm/glm.hpp>
#include <limits>

namespace ge
{
	class Camera final
	{
	public:
		Camera(const glm::vec2& viewportSize);
		~Camera() = default;

		void SetPosition(const glm::vec2& worldPos);
		const glm::vec2& GetPosition() const noexcept { return m_Position; }

		void SetBounds(const glm::vec2& min, const glm::vec2& max);

		// The offset to add to a WORLD position to get the VIEW position
		glm::vec2 GetRenderOffset() const noexcept;

	private:
		glm::vec2 m_Position{ 0.f, 0.f };
		glm::vec2 m_ViewportSize;
		glm::vec2 m_ViewportCenter;

		static inline constexpr float floatInfinity = std::numeric_limits<float>().infinity();

		glm::vec2 m_BoundsMin{ -floatInfinity, -floatInfinity };
		glm::vec2 m_BoundsMax{ floatInfinity, floatInfinity };
		bool m_HasBounds{ false };

		void ApplyBounds();
	};
}