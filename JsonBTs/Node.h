#pragma once
#include "json.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <type_traits>

class BTs;
typedef std::shared_ptr<BTs> SBTs;
typedef std::weak_ptr<BTs> WBTs;

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
	void AssignTree(WBTs tree);

	virtual void AssignParent(WNode parent);
	virtual State Tick();
	virtual SNode Clone(WBTs tree);
	virtual void Load(nlohmann::json& input, WBTs tree);
	virtual const char* GetType() { return ""; }
	virtual void OnInterrupted() = 0;
	virtual ~Node() = default;
protected:
	WNode _parent;
	WNode _this;
	WBTs _tree;
	std::string _type;
};

class LeafNode : public Node
{
public:
	virtual void CustomLoad(nlohmann::json& input) = 0;
	void OnInterrupted() override {}
private:
	void Load(nlohmann::json& input, WBTs tree) override;
};
#define LEAF_DEFAULT_CLONE(type) SNode Clone(WBTs tree) override {auto newNode = Node::Clone(tree); auto rawPtr = dynamic_cast<type*>(newNode.get()); *rawPtr = *this; return newNode;}

class BodyNode : public Node
{
public:
protected:
	SNode CreateChild(const std::string& name);
	inline static std::string GetCreateString(const std::string typeName) {
		return typeName;
	}
};

class CompositeNode : public BodyNode
{
public:
	~CompositeNode() override;
	SNode Clone(WBTs tree) override;
protected:
	void Load(nlohmann::json& input, WBTs tree) override;
	void OnInterrupted() override;
protected:
	std::vector<SNode> _children;
};

class DecoratorNode : public BodyNode
{
protected:
	SNode Clone(WBTs tree) override;
	void Load(nlohmann::json& input, WBTs tree) override;
	void OnInterrupted() override;
protected:
	SNode _child;
};

template<typename T>
void* CreateNode()
{
	return new T();
}

#define NODE_REGISTER(type) private: const char* GetType() override { return #type; } struct RuntimeConstructor{RuntimeConstructor(){NodeFactory::NodeRegister<type> _temp(#type);}}; inline static RuntimeConstructor _runtimeConstructor
class NodeFactory
{
public:
	template<typename T>
	class NodeRegister
	{
	public:
		NodeRegister(const char* name) {
			NodeFactory::Register<T>(name);
		}
	public:
		static_assert(std::is_base_of<Node, T>::value, "You are registering a class that doesn't inherited from Node");
	};
public:
	inline static std::shared_ptr<Node> Create(const std::string& name);
protected:
	template<typename T>
	inline static void Register(const char* name);
	inline static std::unordered_map<std::string, void* (*)()> _functions;
};

template<typename T>
inline void NodeFactory::Register(const char* name)
{
	if (auto result = _functions.emplace(name, &CreateNode<T>); !result.second)
	{
		// Replace later.
		throw std::exception();
	}
}
