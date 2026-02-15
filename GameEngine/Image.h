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
		static constexpr ComponentTypeID StaticTypeID{ 2 };

		Image() = default;
		virtual ~Image() override;

		void SetTexture(Texture2D* textureRef);
		Texture2D* GetTexture() const { return m_Texture; }

	private:
		Texture2D* m_Texture{ nullptr };
	};
}