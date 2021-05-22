#pragma once
#include "NodeFactory.h"
#include "json.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <type_traits>

constexpr auto typeField = "Type";
constexpr auto inputField = "Input";
constexpr auto childrenField = "Children";

class Node;
typedef std::shared_ptr<Node> SNode;
typedef std::weak_ptr<Node> WNode;

class Node
{
public:
	enum class State
	{
		Success,
		Running,
		Failure
	};
public:
	void AssignPtr(WNode ptr);

	virtual void AssignParent(WNode parent);
	virtual State Tick() = 0;
	virtual void Load(nlohmann::json& input) = 0;
	virtual ~Node() = default;
protected:
	WNode _parent;
	WNode _this;
};

class LeafNode : Node
{
public:
	virtual void CustomLoad(nlohmann::json& input) = 0;
private:
	void Load(nlohmann::json& input) override;
};

class BodyNode : public Node
{
protected:
	SNode CreateChild(const std::string& name);
	inline static std::string GetCreateString(const std::string typeName) {
		return typeName + "Node";
	}
};

class CompositeNode : public BodyNode
{
public:
	~CompositeNode() override;
protected:
	void Load(nlohmann::json& input) override;
	std::vector<SNode> _children;
};

class DecoratorNode : public BodyNode
{
protected:
	void Load(nlohmann::json& input) override;
	SNode _child;
};