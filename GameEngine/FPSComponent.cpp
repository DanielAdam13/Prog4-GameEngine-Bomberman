#include "FPSComponent.h"
#include "GameObject.h"
#include "GameEngine.h"
#include "TextComponent.h"

#include <sstream>
#include <iomanip>

using namespace ge;

FPSComponent::FPSComponent(GameObject* pOwnerPtr)
	:Component::Component(pOwnerPtr)
{
}

void FPSComponent::UpdateComponent(float deltaTime)
{
	if (!m_pTextComponent)
		m_pTextComponent = GetOwner()->GetComponent<TextComponent>();

	m_ChangeTimer += deltaTime;
	if (m_ChangeTimer >= ChangeInterval)
	{
		ComputeFPS(deltaTime);

		// Guard against nullptr TextComponent (if GameObject doesn't have TextComponent)
		if (m_pTextComponent)
		{
			// No format because of emscripten
			std::ostringstream ss;
			ss << std::fixed << std::setprecision(2) << m_CurrentFPS << " FPS";
			m_pTextComponent->SetText(ss.str());
		}
			
	}
}

void FPSComponent::ComputeFPS(float deltaTime)
{
	m_FpsTimer += deltaTime;
	++m_FrameCount;

	if (m_FpsTimer >= 1.f)
	{
		m_CurrentFPS = static_cast<float>(m_FrameCount) / m_FpsTimer;

		m_FrameCount = 0;
		m_FpsTimer = 0.f;
	}
}
