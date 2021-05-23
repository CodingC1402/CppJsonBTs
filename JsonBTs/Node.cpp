#include "Node.h"
#include "SelectorNode.h"
#include "ParalelNode.h"
#include "SequenceNode.h"

CompositeNode::~CompositeNode()
{
	_children.clear();
}

void CompositeNode::Load(nlohmann::json& input)
{
	SNode newChild;
	nlohmann::json childInfo;
	for (auto it = input[childrenField].begin(); it != input[childrenField].end(); ++it)
	{
		childInfo = it.value();
		newChild = CreateChild(GetCreateString(childInfo[typeField].get<std::string>()));
		_children.push_back(newChild);
		newChild->Load(childInfo);
	}
}

void CompositeNode::OnInterrupted()
{
	for (const auto& child : _children)
		child->OnInterrupted();
}

Node* NodeFactory::Create(const std::string& name)
{
	return static_cast<Node*>((*_functions[name])());
}

void DecoratorNode::Load(nlohmann::json& input)
{
	auto childInfo = input[childrenField].begin().value();
	_child = CreateChild(GetCreateString(childInfo[typeField].get<std::string>()));
	_child->Load(childInfo);
}

void DecoratorNode::OnInterrupted()
{
	_child->OnInterrupted();
}

void Node::AssignPtr(WNode ptr)
{
	_this = ptr;
}

SNode BodyNode::CreateChild(const std::string& name)
{
	SNode newChild(NodeFactory::Create(name));
	newChild->AssignPtr(newChild);
	newChild->AssignParent(_this);
	return newChild;
}

void Node::AssignParent(WNode parent)
{
	_parent = parent;
}

void LeafNode::Load(nlohmann::json& input)
{
	CustomLoad(input[inputField]);
}

void NodeFactory::RuntimeConstructor::DecoyFunction()
{
	Node* _temp = new Paralel();
	delete _temp;
	_temp = new Selector();
	delete _temp;
	_temp = new Sequence();
	delete _temp;
}
