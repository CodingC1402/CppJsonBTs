#include "ServiceNodes.h"

Node::State AlwaysFail::Tick()
{
    _child->Tick();
    return Node::State::Failure;
}

Node::State AlwaysSuccess::Tick()
{
    _child->Tick();
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
}
