#include "NodeFactory.h"
#include "Node.h"
#include "ParalelNode.h"
#include "SelectorNode.h"
#include "SequenceNode.h"

void NodeFactory::RuntimeConstructor::DecoyFunction()
{
	Node* _temp = new ParalelNode();
	delete _temp;
	_temp = new SelectorNode();
	delete _temp;
	_temp = new SequenceNode();
	delete _temp;
}
