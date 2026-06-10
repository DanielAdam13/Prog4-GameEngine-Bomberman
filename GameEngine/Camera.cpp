#include "Camera.h"
#include "Renderer.h"

#include <algorithm>

ge::Camera::Camera(const glm::vec2& viewportSize)
	:m_ViewportSize{ viewportSize },
	m_ViewportCenter{ viewportSize * 0.5f }
{
}

ge::Camera::~Camera()
{
	// Clear self from Renderer
	if (ge::Renderer::GetInstance().GetActiveCamera() == this)
	{
		Renderer::GetInstance().SetActiveCamera(nullptr);
	}
}

void ge::Camera::SetPosition(const glm::vec2& worldPos)
{
	m_Position = worldPos;

	if (m_HasBounds)
		ApplyBounds();
}

void ge::Camera::SetBounds(const glm::vec2& min, const glm::vec2& max)
{
	m_BoundsMin = min;
	m_BoundsMax = max;

	m_HasBounds = true;
}

glm::vec2 ge::Camera::GetRenderOffset() const noexcept
{
	return m_ViewportCenter - m_Position;
}

void ge::Camera::ApplyBounds()
{
	const auto halfView{ m_ViewportSize * 0.5f };
	const auto mapSize{ m_BoundsMax - m_BoundsMin };

	// X axis
	if (mapSize.x <= m_ViewportSize.x)
	{
		m_Position.x = m_BoundsMin.x + mapSize.x * 0.5f; // Center of map
	}
	else
	{
		m_Position.x = std::clamp(m_Position.x,
			m_BoundsMin.x + halfView.x, m_BoundsMax.x - halfView.x);
	}

	// Y axis
	if (mapSize.y <= m_ViewportSize.y)
	{
		m_Position.y = m_BoundsMin.y + mapSize.y * 0.5f; // Center of map
	}
	else
	{
		m_Position.y = std::clamp(m_Position.y,
			m_BoundsMin.y + halfView.y, m_BoundsMax.y - halfView.y);
	}
}
