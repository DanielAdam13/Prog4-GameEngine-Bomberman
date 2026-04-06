#pragma once
#include <vector>
#include "Observer.h"

namespace ge
{
	class GameObject;

	class Subject final
	{
	public:
		explicit Subject();
		~Subject() = default;

		void AddObserver(IObserver* observer);
		void RemoveObserver(IObserver* observer);

		void NotifyObservers(int eventId, ge::GameObject* sourceObject);

	private:
		std::vector<IObserver*> m_SubjectObservers{};

	};
}