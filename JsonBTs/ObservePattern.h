#pragma once

#include <list>

class Observer
{
public:
	virtual void OnChange() = 0;
protected:
	Observable* _oberserving = nullptr;
};

class Observable
{
public:
	virtual void Subscribe(Observer* observer)
	{
		_observers.push_back(observer);
	}
	virtual void CallOnChange() {
		for (const auto& observer : _observers)
		{
			observer->OnChange();
		}
	}
protected:
	std::list<Observer*> _observers;
};