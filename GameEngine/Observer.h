#pragma once

#include <vector>

namespace ge
{
	class GameObject;
	class Subject;

	// Interface that allows any user to override the Notify method (for custom event outcomes)
	class IObserver
	{
	public:
		IObserver() = default;
		virtual ~IObserver(); // No default since an IObserver auto-unsubscribes itself for each SUBJECT
		IObserver(const IObserver&) = delete;
		IObserver& operator=(const IObserver&) = delete;
		IObserver(IObserver&&) = delete;
		IObserver& operator=(IObserver&&) = delete;

		virtual void Notify(int eventId, ge::GameObject* sourceObject) = 0;

	protected:

	private:
		friend class Subject;

		void RegisterSubject(Subject* s);
		void UnregisterSubject(Subject* s);
		std::vector<Subject*> m_ObservedSubjects; // References to any Subject subscribed to this Obseerver

	};
}