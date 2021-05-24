#pragma once
#include "Node.h"
#include "BlackBoard.h"
#include "Macros.h"

template<typename T>
class Condition : public DecoratorNode
{
public:
	enum class Type {
		Greater,
		GreaterOrEqual,
		Lesser,
		LesserOrEqual,
		Equal,
		NonEqual
	};
public:
	inline void Load(nlohmann::json& input) override {
		switch (_typeStrings[input[BTField::inputField][BTField::typeField].get<std::string>()])
		{
		case Type::Greater:
			_compareFunction = &CompareGreater;
			break;
		case Type::GreaterOrEqual:
			_compareFunction = &CompareGreaterOrEqual;
			break;
		case Type::Lesser:
			_compareFunction = &CompareLesser;
			break;
		case Type::LesserOrEqual:
			_compareFunction = &CompareLesserOrEqual;
			break;
		[[likely]] case Type::Equal:
			_compareFunction = &CompareEqual;
			break;
		[[likely]] case Type::NonEqual:
			_compareFunction = &CompareNonEqual;
			break;
		}
		DecoratorNode::Load(input);
	}
	inline Node::State Tick() override {
		if ((*_compareFunction)(_field, _value)) {
			auto result = _child->Tick();
			if (result == State::Running)
				isRunning = true;
			else
				isRunning = false;
			return result;
		}
	}
protected:
	inline static bool CompareGreater(std::shared_ptr<Field<T>> field, T value) {
		return (*field.get()) > value;
	}
	inline static bool CompareGreaterOrEqual(std::shared_ptr<Field<T>> field, T value) {
		return (*field.get()) >= value;
	}
	inline static bool CompareLesser(std::shared_ptr<Field<T>> field, T value) {
		return (*field.get()) < value;
	}
	inline static bool CompareLesserOrEqual(std::shared_ptr<Field<T>> field, T value) {
		return (*field.get()) <= value;
	}
	inline static bool CompareEqual(std::shared_ptr<Field<T>> field, T value) {
		return (*field.get()) == value;
	}
	inline static bool CompareNonEqual(std::shared_ptr<Field<T>> field, T value) {
		return (*field.get()) != value;
	}
	STATIC_CONSTRUCTOR(
		_typeStrings.emplace("Greater", Type::Greater);
		_typeStrings.emplace("GreaterOrEqual", Type::GreaterOrEqual);
		_typeStrings.emplace("Lesser", Type::Lesser);
		_typeStrings.emplace("LesserOrEqual", Type::LesserOrEqual);
		_typeStrings.emplace("Equal", Type::Equal);
		_typeStrings.emplace("NonEqual", Type::NonEqual);
	)
protected:
	bool (*_compareFunction)(std::shared_ptr<Field<T>>, T);

	bool isRunning = false;
	std::shared_ptr<Field<T>> _field;
	T _value;

	inline static std::unordered_map<std::string, Type> _typeStrings;
};