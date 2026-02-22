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
		const float fps{ GameEngine::GetInstance().GetFPS() };

		// Guard against nullptr TextComponent (if GameObject doesn't have TextComponent)
		if (m_pTextComponent)
		{
			// No format because of emscripten
			std::ostringstream ss;
			ss << std::fixed << std::setprecision(2) << fps << " FPS";
			m_pTextComponent->SetText(ss.str());
		}
			
	}
}
