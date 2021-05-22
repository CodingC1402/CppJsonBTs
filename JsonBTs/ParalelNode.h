#pragma once
#include "Node.h"
#include <list>

class ParalelNode : public CompositeNode
{
public:
	State Tick() override;
protected:
	std::list<WNode> _runningNodes;
	State _tickResult = State::Running;
private:
	NODE_REGISTER(ParalelNode);
};

