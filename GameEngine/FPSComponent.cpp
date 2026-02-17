#include "FPSComponent.h"
#include "GameObject.h"
#include "GameEngine.h"
#include "TextComponent.h"

using namespace ge;

FPSComponent::FPSComponent()
	:m_pTextComponent{ nullptr },
	m_ChangeTimer{ 0.f }
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

		// Guard against nullptr TextComponent (GameObject doesn't have TextComponent)
		if (m_pTextComponent)
			m_pTextComponent->SetText("FPS: " + std::to_string(fps));
	}
}
