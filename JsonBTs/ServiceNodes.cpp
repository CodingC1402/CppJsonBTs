#include "ServiceNodes.h"
#include "Macros.h"

using namespace BTField;

Node::State AlwaysFail::Tick()
{
    if (_child->Tick() == Node::State::Running)
    {
        _runningNode = _child;
        return Node::State::Running;
    }
    _runningNode.reset();
    return Node::State::Failure;
}

Node::State AlwaysSuccess::Tick()
{
    if (_child->Tick() == Node::State::Running)
    {
        _runningNode = _child;
        return Node::State::Running;
    }
    _runningNode.reset();
    return Node::State::Success;
}

Node::State Reverse::Tick()
{
    switch (_child->Tick())
    {
    case State::Failure:
        _runningNode.reset();
        return State::Success;
    case State::Success:
        _runningNode.reset();
        return State::Failure;
    case State::Running:
        _runningNode = _child;
        return State::Running;
    }
}

void ServiceNode::Load(nlohmann::json& input, WBTs tree)
{
    DecoratorNode::Load(input, tree);
    LoadInput(input[inputField]);
}

Node::State Loop::Tick()
{
    if (_currentLoop == 0)
        _result = Node::State::Success;
    
    switch (_child->Tick())
    {
    case Node::State::Failure:
        _result = Node::State::Failure;
        [[fallthrough]];
    case Node::State::Success:
        _runningNode.reset();
        _currentLoop++;
        break;
    case Node::State::Running:
        _runningNode = _child;
    }

    if (_currentLoop == _loopTime)
    {
        _currentLoop = 0;
        return _result;
    }
    return Node::State::Running;
}

SNode Loop::Clone(WBTs tree)
{
    auto clone = DecoratorNode::Clone(tree);
    auto rawPtr = dynamic_cast<Loop*>(clone.get());
    rawPtr->_loopTime = _loopTime;
    return clone;
}

void Loop::LoadInput(nlohmann::json& input)
{
    _loopTime = input["LoopTime"].get<unsigned>();
}

void Loop::OnInterrupted()
{
    DecoratorNode::OnInterrupted();
    _runningNode.reset();
    _loopTime = 0;
    _currentLoop = 0;
}
