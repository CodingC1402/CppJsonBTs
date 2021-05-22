#pragma once
#include "Node.h"

class SelectorNode : public CompositeNode
{
public:
	State Tick() override;
protected:
	int _runningNode = 0;
private:
	NODE_REGISTER(SelectorNode);
};

