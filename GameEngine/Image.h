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

		//Image() = default;
		//virtual ~Image() override = default;

		void SetTexture(std::shared_ptr<Texture2D> texture);
		Texture2D* GetTexture() const { return m_Texture.get(); }

	private:
		std::shared_ptr<Texture2D> m_Texture{};
	};
}