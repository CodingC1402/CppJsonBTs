#include "SequenceNode.h"

Node::State Sequence::Tick()
{
    State result = _children[_runningNode]->Tick();
    switch (result)
    {
    case State::Failure:
        _runningNode = 0;
        [[fallthrough]];
    case State::Running:
        return result;
        break;
    case State::Success:
        _runningNode++;
        _runningNode %= _children.size();
        if (_runningNode == 0)
            return State::Success;
        else
            return Tick();
        break;
    }
}
