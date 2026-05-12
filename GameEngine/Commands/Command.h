#pragma once

namespace ge
{
	class GameObject;

	class Command
	{
	public:
		Command() = default;
		virtual ~Command() = default;

		virtual void Execute(float) = 0;

		virtual GameObject* GetCommandTarget() const noexcept { return nullptr; }

	protected:

	private:

	};
}