#pragma once
#include "Components/Component.h"
#include "ISelectable.h"

#include <SDL3/SDL_pixels.h>
#include <functional>

namespace ge
{
	class TextComponent;
	class GameObject;
}

namespace bombGame
{
	class SelectableTextComponent final : public ge::Component, public ge::ISelectable
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every PlayerComponent Instance shares the same component type ID
		static constexpr ge::ComponentTypeID StaticTypeID{ 23 };

		SelectableTextComponent(ge::GameObject* owner, const SDL_Color& normalColor, const SDL_Color& selectedColor,
			std::function<void()> selectionFunction);
		~SelectableTextComponent() override = default;
		SelectableTextComponent(const SelectableTextComponent&) = delete;
		SelectableTextComponent(SelectableTextComponent&&) = delete;
		SelectableTextComponent& operator=(const SelectableTextComponent&) = delete;
		SelectableTextComponent& operator=(SelectableTextComponent&&) = delete;

		void FixedUpdateComponent(float) override {} // nothing
		void UpdateComponent(float) override {} // nothing
		void RenderComponent() const override {} // nothing

		virtual void OnSelected() override;
		virtual void OnHover() override;
		virtual void OnExit() override;

	private:
		ge::TextComponent* m_CachedText{ nullptr }; // Cached ref from owner

		const SDL_Color m_NormalTextColor;
		const SDL_Color m_SelectedTextColor;

		const std::function<void()> m_SelectionFunction;
	};
}