#include "ObservableSubject.h"
#include "GameObject.h"

#include <vector>

using namespace ge;

Subject::~Subject()
{
	// Intentional copy because we mutate m_ObservedSubject while doing this
	auto observers = m_SubjectObservers;
	for (auto* o : observers)
	{
		o->UnregisterSubject(this);
	}	
}

void Subject::AddObserver(IObserver* observer)
{
	m_SubjectObservers.push_back(observer);
	observer->RegisterSubject(this);
}

void Subject::RemoveObserver(IObserver* observer)
{
	std::erase(m_SubjectObservers, observer);
	observer->UnregisterSubject(this);
}

void Subject::NotifyObservers(int eventId, ge::GameObject* sourceObject)
{
	auto observers = m_SubjectObservers;
	for (auto* observer : observers)
	{
		observer->Notify(eventId, sourceObject);
	}
}
