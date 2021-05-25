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
	inline SNode Clone(WBTs tree) override {
		auto cloneNode = std::dynamic_pointer_cast<Condition<T>>(DecoratorNode::Clone(tree));
		cloneNode->_compareFunction = _compareFunction;
		cloneNode->_value = _value;
		cloneNode->_isTrue = _isTrue;
		cloneNode->_field = std::dynamic_pointer_cast<Field<T>>(tree.lock()->GetBlackBoard().lock()->GetField<T>(_field.lock()->GetFieldName()).lock());
		cloneNode->_field.lock()->Subscribe(cloneNode.get());
		return cloneNode;
	}
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
		_field = std::dynamic_pointer_cast<Field<T>>(tree.lock()->GetBlackBoard().lock()->GetField<T>(input[BTField::inputField][BTField::fieldField].get<std::string>()).lock());
		_field.lock()->Subscribe(this);
		_isTrue = (*_compareFunction)(_field.lock(), _value);
		DecoratorNode::Load(input, tree);
	}
	inline void OnChange() override {
		_isTrue = (*_compareFunction)(_field.lock(), _value);
		if (_isRunning && !_isTrue)
		{
			OnInterrupted();
			_isRunning = false;
			_parent.lock()->Tick();
		}
	}
	inline Node::State Tick() override {
		if (_isTrue) {
			auto result = _child->Tick();
			if (result == State::Running) {
				_isRunning = true;
				_runningNode = _child;
			}
			else {
				_isRunning = false;
				_runningNode.reset();
			}
			return result;
		}
		else
			return State::Failure;
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

	bool _isTrue = false;
	bool _isRunning = false;
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
