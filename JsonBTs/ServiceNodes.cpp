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
        break;
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
    _loopTime = input[loopField].get<unsigned>();
}

void Loop::OnInterrupted()
{
    DecoratorNode::OnInterrupted();
    _currentLoop = 0;
}

Node::State Wait::Tick()
{
    if (!_isRunning)
    {
        _start = std::chrono::system_clock::now();
        _isRunning = true;
        return Node::State::Running;
    }
    else
    {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> timepassed = now - _start;
        if (timepassed.count() * 1000 >= _waitMs)
        {
            auto result = _child->Tick();
            switch (result)
            {
            case Node::State::Failure:
                [[fallthrough]];
            case Node::State::Success:
                _isRunning = false;
                _runningNode.reset();
                break;
            case Node::State::Running:
                if (_onceEachTick)
                    _isRunning = false;
                _runningNode = _child;
                break;
            }
            return result;
        }
        else
            return Node::State::Running;
    }
}

SNode Wait::Clone(WBTs tree)
{
    auto cloneNode = ServiceNode::Clone(tree);
    auto cloneNodeDerived = std::dynamic_pointer_cast<Wait>(cloneNode);
    cloneNodeDerived->_waitMs = _waitMs;
    cloneNodeDerived->_onceEachTick = _onceEachTick;
    return cloneNode;
}

void Wait::LoadInput(nlohmann::json& input)
{
    _waitMs = input[durationField].get<unsigned>();
    if (input[oncePerTickField] != nullptr)
        _onceEachTick = input[oncePerTickField].get<bool>();
    else
        _onceEachTick = false;
}

void Wait::OnInterrupted()
{
    ServiceNode::OnInterrupted();
    _isRunning = false;
}
