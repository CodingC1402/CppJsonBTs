#include "ParalelNode.h"

Node::State Paralel::Tick()
{
    if (_runningNodes.empty())
    {
        _tickResult = State::Success;
        for (const auto& child : _children)
        {
            _runningNodes.emplace_back(child);
        }
    }

    State result;
    for (auto it = _runningNodes.begin(); it != _runningNodes.end();)
    {
        result = (*it).lock()->Tick();
        switch (result)
        {
        case State::Running:
            ++it;
            break;
        case State::Failure:
            _tickResult = State::Failure;
            [[fallthrough]];
        case State::Success:
            auto eraseIt = it;
            ++it;
            _runningNodes.erase(eraseIt);
            break;
        }
    }

    if (_runningNodes.empty())
        return _tickResult;
    else
        return State::Running;
}

void Paralel::OnInterrupted()
{
    for (const auto& node : _runningNodes)
        node.lock()->OnInterrupted();
    _runningNodes.clear();
}
