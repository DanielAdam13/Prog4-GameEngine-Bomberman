#pragma once
#include "Texture2D.h"
#include "Component.h"
#include <memory>

namespace ge
{
	class Image final : public Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Image Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 1 };

		Image(GameObject* pOwnerPtr);
		virtual ~Image() override = default;

		// Texture is renderer INSIDE an individual Image component
		virtual void RenderComponent() const override;
		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override {};

		void SetTexture(Texture2D* textureRef);
		Texture2D* GetTexture() const noexcept { return m_pTexture; }

	private:
		Texture2D* m_pTexture{ nullptr }; // Doesn't own
	};
}