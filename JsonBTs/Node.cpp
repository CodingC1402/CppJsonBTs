#include "Node.h"
#include "SelectorNode.h"
#include "ParalelNode.h"
#include "SequenceNode.h"
#include "ServiceNodes.h"
#include "Condition.h"
#include "BTs.h"
#include "Macros.h"

using namespace BTField;

CompositeNode::~CompositeNode()
{
	_children.clear();
}

SNode CompositeNode::Clone(WBTs tree)
{
	auto newNode = Node::Clone(tree);
	auto newNodeRawPtr = dynamic_cast<CompositeNode*>(newNode.get());
	for (const auto& child : _children)
	{
		auto childClone = child->Clone(tree);
		childClone->AssignParent(newNode);
		newNodeRawPtr->_children.push_back(childClone);
	}
	return newNode;
}

void CompositeNode::Load(nlohmann::json& input, WBTs tree)
{
	Node::Load(input, tree);
	SNode newChild;
	nlohmann::json childInfo;
	for (auto it = input[childrenField].begin(); it != input[childrenField].end(); ++it)
	{
		childInfo = it.value();
		newChild = CreateChild(GetCreateString(childInfo[typeField].get<std::string>()));
		_children.push_back(newChild);
		newChild->Load(childInfo, tree);
	}
}

std::shared_ptr<Node> NodeFactory::Create(const std::string& name)
{
	auto found = _functions.find(name);
	if (found == _functions.end())
	{
		// Change exception later
		throw std::exception();
	}

	auto newNode = SNode(static_cast<Node*>((*_functions[name])()));
	newNode->AssignPtr(newNode);
	return newNode;
}

SNode DecoratorNode::Clone(WBTs tree)
{
	auto newNode = Node::Clone(tree);
	auto newNodeRawPtr = dynamic_cast<DecoratorNode*>(newNode.get());

	auto childClone = _child->Clone(tree);
	childClone->AssignParent(newNode);
	newNodeRawPtr->_child = childClone;
	return newNode;
}

void DecoratorNode::Load(nlohmann::json& input, WBTs tree)
{
	Node::Load(input, tree);
	auto childInfo = input[childrenField].begin().value();
	_child = CreateChild(GetCreateString(childInfo[typeField].get<std::string>()));
	_child->Load(childInfo, tree);
}

void Node::AssignPtr(WNode ptr)
{
	_this = ptr;
}

void Node::AssignTree(WBTs tree)
{
	_tree = tree;
}

void BodyNode::OnInterrupted()
{
	if (!_runningNode.expired())
		_runningNode.lock()->OnInterrupted();
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

SNode Node::Clone(WBTs tree)
{
	SNode newNode;
	newNode = NodeFactory::Create(GetType());
	newNode->AssignTree(tree);
	return newNode;
}

void Node::Load(nlohmann::json& input, WBTs tree)
{
	_tree = tree;
}

void LeafNode::Load(nlohmann::json& input, WBTs tree)
{
	Node::Load(input, tree);
	CustomLoad(input[inputField]);
}
