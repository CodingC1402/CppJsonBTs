#include "BlackBoard.h"
#include "Macros.h"
#include <fstream>

using namespace BTField;

void FieldBase::AssignPtr(const std::weak_ptr<FieldBase>& ptr)
{
	_this = ptr;
}

SField FieldFactory::Load(nlohmann::json& input)
{
	auto type = input[typeField].get<std::string>();
	auto value = input[valueField];
	return (*_createFunction[type])(value);
}

std::shared_ptr<BlackBoard> BlackBoard::Load(nlohmann::json& input)
{
	auto newBoard = std::make_shared<BlackBoard>();
	for (auto& field : input)
		newBoard->_fields.emplace(field[BTField::nameField].get<std::string>(), FieldFactory::Load(field));
	return newBoard;
}
