#include "BTs.h"
#include "Macros.h"
#include <fstream>

BTs::BTs()
{
	_root = NodeFactory::Create("Root");
}

SBTs BTs::Load(const std::string& path)
{
	std::fstream file;
	file.open(path);
	nlohmann::json input;
	file >> input;

	auto newTree = std::make_shared<BTs>();
	newTree->AssignPtr(newTree);
	newTree->_bb = BlackBoard::Load(input[BTField::blackBoardField]);
	newTree->_root->Load(input, newTree->_this);
	return newTree;
}

void BTs::AssignPtr(WBTs ptr)
{
	_this = ptr;
}

Node::State BTs::Tick()
{
	return _root->Tick();
}

WBlackBoard BTs::GetBlackBoard()
{
	return _bb;
}

SBTs BTs::Clone()
{
	auto newTree = std::make_shared<BTs>();
	newTree->_root = _root->Clone(newTree);
	return newTree;
}

Node::State Root::Tick()
{
	return _child->Tick();
}
