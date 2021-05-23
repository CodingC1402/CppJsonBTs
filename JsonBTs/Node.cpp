#include "Node.h"
#include "SelectorNode.h"
#include "ParalelNode.h"
#include "SequenceNode.h"
#include "ServiceNodes.h"
#include "Macros.h"

using namespace BTField;

CompositeNode::~CompositeNode()
{
	_children.clear();
}

SNode CompositeNode::Clone()
{
	auto newNode = Node::Clone();
	auto newNodeRawPtr = dynamic_cast<CompositeNode*>(newNode.get());
	for (const auto& child : _children)
	{
		auto childClone = child->Clone();
		childClone->AssignParent(newNode);
		newNodeRawPtr->_children.push_back(childClone);
	}
	return newNode;
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

std::shared_ptr<Node> NodeFactory::Create(const std::string& name)
{
	auto newNode = SNode(static_cast<Node*>((*_functions[name])()));
	newNode->AssignPtr(newNode);
	return newNode;
}

SNode DecoratorNode::Clone()
{
	auto newNode = Node::Clone();
	auto newNodeRawPtr = dynamic_cast<DecoratorNode*>(newNode.get());

	auto childClone = _child->Clone();
	childClone->AssignParent(newNode);
	newNodeRawPtr->_child = childClone;
	return newNode;
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
	SNode newChild = NodeFactory::Create(name);
	newChild->AssignParent(_this);
	return newChild;
}

void Node::AssignParent(WNode parent)
{
	_parent = parent;
}

Node::State Node::Tick()
{
	return Node::State::Success;
}

SNode Node::Clone()
{
	SNode newNode;
	newNode = NodeFactory::Create(GetType());
	return newNode;
}

void LeafNode::Load(nlohmann::json& input)
{
	CustomLoad(input[inputField]);
}
