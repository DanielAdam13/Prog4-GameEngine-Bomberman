#include "FPSComponent.h"
#include "GameObject.h"
#include "GameEngine.h"
#include "TextComponent.h"

#include <format>

using namespace ge;

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
			m_pTextComponent->SetText(std::format("{:.2f} FPS", fps));
		}
			
	}
}
