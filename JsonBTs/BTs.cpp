#include "BTs.h"
#include <fstream>

BTs::BTs()
{
	_root = std::make_shared<RootNode>();
	_root->AssignPtr(_root);
}

void BTs::Load(const std::string& path)
{
	std::fstream file;
	file.open(path);
	nlohmann::json Input;
	file >> Input;

	_root.reset(new RootNode());
	_root->AssignPtr(_root);
	_root->Load(Input);
}

Node::State BTs::Tick()
{
	return _root->Tick();
}

Node::State RootNode::Tick()
{
	return _child->Tick();
}
