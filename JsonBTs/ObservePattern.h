#pragma once

#include <list>

class Observable;
class Observer;

class Observer
{
public:
	virtual void OnChange() = 0;
	virtual ~Observer();
protected:
	Observable* _oberserving = nullptr;
};

class Observable
{
public:
	virtual void UnSubscribe(Observer* observer);
	virtual void Subscribe(Observer* observer);
	virtual void CallOnChange();
protected:
	std::list<Observer*> _observers;
};