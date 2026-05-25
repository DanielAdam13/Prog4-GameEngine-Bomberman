#pragma once
#include <vector>
#include "Observer.h"

namespace ge
{
	class GameObject;

	class Subject final
	{
	public:
		Subject() = default;
		~Subject();
		Subject(const Subject&) = delete;
		Subject& operator=(const Subject&) = delete;
		Subject(Subject&&) = delete;
		Subject& operator=(Subject&&) = delete;

		void AddObserver(IObserver* observer);
		void RemoveObserver(IObserver* observer);
		void NotifyObservers(int eventId, ge::GameObject* sourceObject);

	private:
		std::vector<IObserver*> m_SubjectObservers{};

	};
}