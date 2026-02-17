#pragma once
#include "Component.h"

namespace ge
{
	class Transform final : public Component
	{
	public:
		// ---- TYPE IDENTIFIER ----
		// Every Transform Instance shares the same component type ID
		static constexpr ComponentTypeID StaticTypeID{ 0 };

		Transform();
		Transform(const glm::vec3 pos, const glm::vec3 rot, const glm::vec3 scale = { 1.f, 1.f, 1.f });

		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override {};
		virtual void RenderComponent(const glm::vec3&) const override {};

		const glm::vec3& GetPosition() const noexcept { return m_Position; }
		void SetPosition(float x, float y, float z = 0);
		void SetPosition(const glm::vec3& position);

		const glm::vec3& GetRotation() const noexcept { return m_Rotation; }
		void SetRotation(float x, float y, float);
		void SetRotation(const glm::vec3& rotation);

		const glm::vec3& GetScale() const noexcept { return m_Scale; }
		void SetScale(float x = 1.f, float y = 1.f, float z = 1.f);
		void SetScale(const glm::vec3& scale = {1.f, 1.f, 1.f});

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_Scale;
	};
}
