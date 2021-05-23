#include "ServiceNodes.h"
#include "Macros.h"

using namespace BTField;

Node::State AlwaysFail::Tick()
{
    if (_child->Tick() == Node::State::Running)
        return Node::State::Running;
    return Node::State::Failure;
}

Node::State AlwaysSuccess::Tick()
{
    if (_child->Tick() == Node::State::Running)
        return Node::State::Running;
    return Node::State::Success;
}

Node::State Reverse::Tick()
{
    switch (_child->Tick())
    {
    case State::Failure:
        return State::Success;
    case State::Success:
        return State::Failure;
    }
    return State::Running;
}

void ServiceNode::Load(nlohmann::json& input)
{
    DecoratorNode::Load(input);
    LoadInput(input[inputField]);
}

Node::State Loop::Tick()
{
    if (_currentLoop == 0)
        _result = Node::State::Success;
    
    if (_child->Tick() == Node::State::Failure)
    {
        _result = Node::State::Failure;
    }
    _currentLoop++;

    if (_currentLoop == _loopTime)
    {
        _currentLoop = 0;
        return _result;
    }
    return Node::State::Running;
}

SNode Loop::Clone()
{
    auto clone = DecoratorNode::Clone();
    auto rawPtr = dynamic_cast<Loop*>(clone.get());
    rawPtr->_loopTime = _loopTime;
    return clone;
}

void Loop::LoadInput(nlohmann::json& input)
{
    _loopTime = input["LoopTime"].get<unsigned>();
}
