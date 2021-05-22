#pragma once
#include "Node.h"

class SequenceNode : public CompositeNode
{
public:
	State Tick() override;
protected:
	int _runningNode = 0;
private:
	NODE_REGISTER(SelectorNode);
};

