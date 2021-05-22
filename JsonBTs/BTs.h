#pragma once
#include "Node.h"

class RootNode : public DecoratorNode
{
public:
	Node::State Tick() override;
};

class BTs
{
public:
	BTs();
	void Load(const std::string& path);
	Node::State Tick();
protected:
	SNode _root;
};

