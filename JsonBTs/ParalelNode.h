#pragma once
#include "Node.h"
#include <list>

class Paralel : public CompositeNode
{
public:
	State Tick() override;
	void OnInterrupted() override;
protected:
	std::list<WNode> _runningNodes;
	State _tickResult = State::Running;
private:
	NODE_REGISTER(Paralel);
};

