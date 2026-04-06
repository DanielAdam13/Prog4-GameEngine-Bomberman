#pragma once

namespace ge
{
	class GameObject;

	class IObserver
	{
	public:
		IObserver() = default;
		virtual ~IObserver() = default;

		virtual void Notify(int eventId, ge::GameObject* sourceObject) = 0;

	protected:

	private:

	};
}