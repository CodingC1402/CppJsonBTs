#include "SelectorNode.h"

Node::State SelectorNode::Tick()
{
    State result = _children[_runningNode]->Tick();
    switch (result) 
    {
    case State::Success:
        _runningNode = 0;
        [[fallthrough]];
    case State::Running:
        return result;
        break;
    case State::Failure:
        _runningNode++;
        _runningNode %= _children.size();
        if (_runningNode == 0)
            return State::Failure;
        else
            return Tick();
        break;
    }
}
