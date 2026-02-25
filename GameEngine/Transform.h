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

		Transform(GameObject* pOwnerPtr);
		Transform(GameObject* pOwnerPtr, const glm::vec3 pos, const glm::vec3 rot, const glm::vec3 scale = { 1.f, 1.f, 1.f });

		virtual void FixedUpdateComponent(float) override {};
		virtual void UpdateComponent(float) override {};
		virtual void RenderComponent() const override {};

		const glm::vec3& GetLocalPosition() const noexcept { return m_LocalPosition; }
		const glm::vec3& GetLocalRotation() const noexcept { return m_LocalRotation; }
		const glm::vec3& GetLocalScale() const noexcept { return m_LocalScale; }

		glm::vec3 GetWorldPosition();
		glm::vec3 GetWorldRotation();
		glm::vec3 GetWorldScale();
		const glm::mat4x4& GetWorldMatrix();

		void SetLocalPosition(float x, float y, float z = 0);
		void SetLocalPosition(const glm::vec3& position);
		
		void SetLocalRotation(float x, float y, float);
		void SetLocalRotation(const glm::vec3& rotation);
		
		void SetLocalScale(float x = 1.f, float y = 1.f, float z = 1.f);
		void SetLocalScale(const glm::vec3& scale = {1.f, 1.f, 1.f});

		void MarkDirty();

	private:
		glm::vec3 m_LocalPosition;
		glm::vec3 m_LocalRotation;
		glm::vec3 m_LocalScale;

		glm::mat4x4 m_LocalMatrix{};
		glm::mat4x4 m_WorldMatrix{};


		bool m_DirtyFlag;

		Transform* GetParentTransform() const;

		// Called only when needed:
		// At Getter functions
		void RecomputeWorldMatrix();
	};
}
