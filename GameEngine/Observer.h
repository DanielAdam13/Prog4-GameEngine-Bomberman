#pragma once

namespace ge
{
	class GameObject;

	// Interface that allows any user to override the Notify method (for custom event outcomes)
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