#include "BlackBoard.h"

void BlackBoard::FieldBase::SetValue(std::shared_ptr<FieldBase>& field)
{
	*this = *field.get();
}
