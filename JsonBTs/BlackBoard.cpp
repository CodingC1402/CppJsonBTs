#include "BlackBoard.h"
#include <fstream>

using namespace temp;

void FieldBase::AssignPtr(const std::weak_ptr<FieldBase>& ptr)
{
	_this = ptr;
}

SField FieldFactory::Load(nlohmann::json& input)
{
	return (*_createFunction[input[typeField].get<std::string>()])(input[valueField]);
}

std::shared_ptr<BlackBoard> BlackBoard::Load(nlohmann::json& input)
{
	auto newBoard = std::make_shared<BlackBoard>();
	for (auto& field : input)
		newBoard->_fields.emplace(field["Name"].get<std::string>(), FieldFactory::Load(field));
	return newBoard;
}
