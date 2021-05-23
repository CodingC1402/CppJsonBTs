#pragma once
#include "Node.h"

class ServiceNode : public DecoratorNode
{
protected:
	virtual void LoadInput(nlohmann::json& input) {};
private:
	void Load(nlohmann::json& input) override;
};

class Reverse : public ServiceNode
{
public: 
	Node::State Tick() override;
private:
	NODE_REGISTER(Reverse);
};

class AlwaysFail : public ServiceNode
{
public:
	Node::State Tick() override;
private:
	NODE_REGISTER(AlwaysFail);
};

class AlwaysSuccess : public ServiceNode
{
public:
	Node::State Tick() override;
private:
	NODE_REGISTER(AlwaysSuccess);
};

class Loop : public ServiceNode
{
public:
	Node::State Tick() override;
	SNode Clone() override;
	void LoadInput(nlohmann::json& input) override;
public:
	unsigned _loopTime = 0;
	unsigned _currentLoop = 0;

	Node::State _result = Node::State::Success;
private:
	NODE_REGISTER(Loop);
};

class Comparator : public DecoratorNode
{

};