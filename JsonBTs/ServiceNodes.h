#pragma once
#include "Node.h"
#include <chrono>

class ServiceNode : public DecoratorNode
{
protected:
	virtual void LoadInput(nlohmann::json& input) {};
private:
	void Load(nlohmann::json& input, WBTs tree) override;
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

class Wait : public ServiceNode
{
public:
	Node::State Tick() override;
	SNode Clone(WBTs tree) override;
	void LoadInput(nlohmann::json& input) override;
	void OnInterrupted() override;
protected:
	unsigned _waitMs = 0;
	bool _isRunning = false;
	std::chrono::system_clock::time_point _start;
private:
	NODE_REGISTER(Wait);
};

class Loop : public ServiceNode
{
public:
	Node::State Tick() override;
	SNode Clone(WBTs tree) override;
	void LoadInput(nlohmann::json& input) override;
	void OnInterrupted() override;
protected:
	unsigned _loopTime = 0;
	unsigned _currentLoop = 0;

	Node::State _result = Node::State::Success;
private:
	NODE_REGISTER(Loop);
};