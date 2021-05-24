#include "BlackBoard.h"
#include "Macros.h"
#include <fstream>

void FieldBase::AssignPtr(const std::weak_ptr<FieldBase>& ptr)
{
	_this = ptr;
}

SField FieldFactory::Load(nlohmann::json& input)
{
	auto type = input[BTField::typeField].get<std::string>();
	return (*_createFunction[type])(input);
}

SField FieldFactory::Clone(WField toClone)
{
	return (*_cloneFunction[toClone.lock()->GetFieldType()])(toClone);
}

std::shared_ptr<BlackBoard> BlackBoard::Load(nlohmann::json& input)
{
	auto newBoard = std::make_shared<BlackBoard>();
	for (auto& field : input)
		newBoard->_fields.emplace(field[BTField::nameField].get<std::string>(), FieldFactory::Load(field));
	return newBoard;
}

SBlackBoard BlackBoard::Clone()
{
	SBlackBoard newBoard = std::make_shared<BlackBoard>();
	for (SField cloneField; const auto& field : _fields)
	{
		cloneField = FieldFactory::Clone(field.second);
		newBoard->_fields.emplace(cloneField->GetFieldName(), cloneField);
	}
	return newBoard;
}
