#include "Observer.h"
#include "ObservableSubject.h"

ge::IObserver::~IObserver()
{
    // Intentional copy because we mutate m_ObservedSubject while doing this
    auto subjects = m_ObservedSubjects;
    for (auto* s : subjects)
    {
        s->RemoveObserver(this); // !! Subjects are auto-removed from an Observer !!
    }
}

void ge::IObserver::RegisterSubject(Subject* s)
{
    m_ObservedSubjects.push_back(s);
}

void ge::IObserver::UnregisterSubject(Subject* s)
{
    std::erase(m_ObservedSubjects, s);
}
