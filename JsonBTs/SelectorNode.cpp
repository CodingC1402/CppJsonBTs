#include "SelectorNode.h"

Node::State Selector::Tick()
{
    State result = _children[_runningNodeIndex]->Tick();
    switch (result) 
    {
    case State::Success:
        _runningNode.reset();
        _runningNodeIndex = 0;
        [[fallthrough]];
    case State::Running:
        _runningNode = _children[_runningNodeIndex];
        return result;
        break;
    case State::Failure:
        _runningNode.reset();
        _runningNodeIndex++;
        _runningNodeIndex %= _children.size();
        if (_runningNodeIndex == 0)
            return State::Failure;
        else
            return Tick();
        break;
    }
}
