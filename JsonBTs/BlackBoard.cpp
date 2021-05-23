#include "BlackBoard.h"

void FieldBase::AssignPtr(const std::weak_ptr<FieldBase>& ptr)
{
	_this = ptr;
}
