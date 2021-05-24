#pragma once
#include "Node.h"

class Selector : public CompositeNode
{
public:
	State Tick() override;
	void OnInterrupted() override;
protected:
	int _runningNodeIndex = 0;
private:
	NODE_REGISTER(Selector);
};

