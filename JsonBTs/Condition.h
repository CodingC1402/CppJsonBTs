#pragma once
#include "BTs.h"
#include "Macros.h"
#include "ObservePattern.h"

template<typename T>
class Condition : public DecoratorNode, public Observer
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
	inline void Load(nlohmann::json& input, WBTs tree) override {
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
		_value = input[BTField::inputField][BTField::valueField].get<T>();
		_field = tree.lock()->GetBlackBoard().lock()->GetField<T>(input[BTField::inputField][BTField::fieldField].get<std::string>());
		_field.lock()->Subscribe(this);
		DecoratorNode::Load(input, tree);
	}
	inline void OnChange() override {
		if (isRunning)
		{
			OnInterrupted();
			isRunning = false;
		}
	}
	inline Node::State Tick() override {
		if ((*_compareFunction)(_field.lock(), _value)) {
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
protected:
	bool (*_compareFunction)(std::shared_ptr<Field<T>>, T);

	bool isRunning = false;
	std::weak_ptr<Field<T>> _field;
	T _value;

	inline static std::unordered_map<std::string, Type> _typeStrings;
private:
	STATIC_CONSTRUCTOR(
		_typeStrings.emplace("Greater", Type::Greater);
		_typeStrings.emplace("GreaterOrEqual", Type::GreaterOrEqual);
		_typeStrings.emplace("Lesser", Type::Lesser);
		_typeStrings.emplace("LesserOrEqual", Type::LesserOrEqual);
		_typeStrings.emplace("Equal", Type::Equal);
		_typeStrings.emplace("NonEqual", Type::NonEqual);
	)
};
#define DEFINE_CONDITION_TYPE(nameForType, templateType) class nameForType : public Condition<templateType> {NODE_REGISTER(nameForType);}

DEFINE_CONDITION_TYPE(StringCon, std::string);
DEFINE_CONDITION_TYPE(CharCon, char);
DEFINE_CONDITION_TYPE(IntCon, int);
DEFINE_CONDITION_TYPE(UnsignedCon, unsigned);
DEFINE_CONDITION_TYPE(FloatCon, float);
DEFINE_CONDITION_TYPE(DoubleCon, double);
DEFINE_CONDITION_TYPE(BoolCon, bool);
DEFINE_CONDITION_TYPE(LongLongCon, long long);
DEFINE_CONDITION_TYPE(UnsignedLongLongCon, unsigned long long);
