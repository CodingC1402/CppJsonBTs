#include "BTs.h"
#include <fstream>

BTs::BTs()
{
	_root = NodeFactory::Create("Root");
}

SBTs BTs::Load(const std::string& path)
{
	std::fstream file;
	file.open(path);
	nlohmann::json Input;
	file >> Input;

	auto newTree = std::make_shared<BTs>();
	newTree->_root->Load(Input);
	return newTree;
}

Node::State BTs::Tick()
{
	return _root->Tick();
}

SBTs BTs::Clone()
{
	auto newTree = std::make_shared<BTs>();
	newTree->_root = _root->Clone();
	return newTree;
}

Node::State Root::Tick()
{
	return _child->Tick();
}
