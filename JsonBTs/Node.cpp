#include "Node.h"
#include "NodeFactory.h"

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
