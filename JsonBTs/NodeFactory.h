#pragma once
#include <memory>
#include <string>
#include <unordered_map>

class Node;
class ParalelNode;
class SelectorNode;
class SequenceNode;

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
		static_assert(std::is_base_of<Node, T>::value, "You are registering a type that is not descendant of class Node");
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
	_functions.emplace(name, &CreateNode<T>);
}
