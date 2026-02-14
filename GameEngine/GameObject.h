#pragma once
#include <string>
#include <memory>

namespace ge
{
	class Texture2D;

	class GameObject final
	{
	public:
		explicit GameObject();
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
		
		void Update();
		void Render() const;
	private:

	};
}