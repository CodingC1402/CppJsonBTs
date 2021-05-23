#pragma once
#include "Node.h"

class ServiceNode : public DecoratorNode
{
protected:
	virtual void LoadInput(nlohmann::json) {};
private:
	void Load(nlohmann::json& input) override;
};

class Reverse : public DecoratorNode
{
public: 
	Node::State Tick() override;
};

class AlwaysFail : public DecoratorNode
{
public:
	Node::State Tick() override;
};

class AlwaysSuccess : public DecoratorNode
{
public:
	Node::State Tick() override;
};

class Loop : public DecoratorNode
{
public:
	Node::State Tick() override;
public:
	int _loopTime = 0;
	int _currentLoop = 0;

	Node::State _result = Node::State::Success;
};