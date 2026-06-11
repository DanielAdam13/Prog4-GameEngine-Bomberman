#include "SelectableTextComponent.h"
#include "GameObject.h"
#include "Components/TextComponent.h"

#include <cassert>

bombGame::SelectableTextComponent::SelectableTextComponent(ge::GameObject* owner, const SDL_Color& normalColor, const SDL_Color& selectedColor,
	std::function<void()> selectionFunction)
	:Component::Component(owner),
	m_CachedText{ owner->GetComponent<ge::TextComponent>() },
	m_NormalTextColor{ normalColor },
	m_SelectedTextColor{ selectedColor },
	m_SelectionFunction{ selectionFunction }
{
	assert(m_CachedText && "SelectableText requires a TextComponent on the same GO");
}

void bombGame::SelectableTextComponent::OnSelected()
{
	m_CachedText->SetColor(m_SelectedTextColor);
	if (m_SelectionFunction)
	{
		m_SelectionFunction();
	}
}

void bombGame::SelectableTextComponent::OnHover()
{
	m_CachedText->SetColor(m_SelectedTextColor);
}

void bombGame::SelectableTextComponent::OnExit()
{
	m_CachedText->SetColor(m_NormalTextColor);
}

void bombGame::SelectableTextComponent::SetText(const std::string& text)
{
	m_CachedText->SetText(text);
}
