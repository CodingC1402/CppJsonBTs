#pragma once
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
	virtual void OnInterrupted() = 0;
	virtual ~Node() = default;
protected:
	WNode _parent;
	WNode _this;
};

class LeafNode : public Node
{
public:
	virtual void CustomLoad(nlohmann::json& input) = 0;
	void OnInterrupted() override {};
private:
	void Load(nlohmann::json& input) override;
};

class BodyNode : public Node
{
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
protected:
	void Load(nlohmann::json& input) override;
	void OnInterrupted() override;
protected:
	std::vector<SNode> _children;
};

class DecoratorNode : public BodyNode
{
protected:
	void Load(nlohmann::json& input) override;
	void OnInterrupted() override;
protected:
	SNode _child;
};

template<typename T>
void* CreateNode()
{
	return new T();
}

#define NODE_REGISTER(type) struct RuntimeConstructor{RuntimeConstructor(){NodeFactory::NodeRegister<type> _temp(#type);}}; inline static RuntimeConstructor _runtimeConstructor
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
	inline static Node* Create(const std::string& name);
protected:
	template<typename T>
	inline static void Register(const char* name);
	inline static std::unordered_map<std::string, void* (*)()> _functions;

private:
	struct RuntimeConstructor
	{
		void DecoyFunction();
	};
	inline static RuntimeConstructor _runTimeConstructor;
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
