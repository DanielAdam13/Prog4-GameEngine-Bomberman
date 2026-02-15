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
		// Every Transform Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 1 };

		Image() = default;
		virtual ~Image() override;

		virtual void RenderComponent(const glm::vec3 transformPos) const override;

		void SetTexture(Texture2D* textureRef);
		Texture2D* GetTexture() const { return m_pTexture; }

	private:
		Texture2D* m_pTexture{ nullptr };
	};
}