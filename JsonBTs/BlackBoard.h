#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "Macros.h"
#include "json.hpp"

class FieldBase;
template<typename T>
class Field;

class FieldBase
{
public:
	void AssignPtr(const std::weak_ptr<FieldBase>& ptr);

	template<typename T>
	static inline std::shared_ptr<FieldBase> Create() {
		std::shared_ptr<FieldBase> newField;
		newField.reset(new Field<T>());
		newField->AssignPtr(newField);
		return newField;
	}
	template<typename T>
	inline void SetValueTemplate(const T& value) {
		static_cast<Field<T>*>(this)->SetValue(value);
	}
	template<typename T>
	inline T GetValueTemplate() {
		return static_cast<Field<T>*>(this)->GetValue();
	}
protected:
	std::weak_ptr<FieldBase> _this;
};
typedef std::weak_ptr<FieldBase> WField;
typedef std::shared_ptr<FieldBase> SField;

template<typename T>
class Field : public FieldBase
{
	friend class BlackBoard;
public:
	inline void SetValue(const T& value) {
		_value = value;
	}
	inline T GetValue() {
		return _value;
	}

#pragma region Operators with value
	inline bool operator== (const T& value) {
		return _value == value;
	}
	inline bool operator!= (const T& value) {
		return _value != value;
	}

	inline bool operator>= (const T& value) {
		return _value >= value;
	}
	inline bool operator<= (const T& value) {
		return _value <= value;
	}
	inline bool operator> (const T& value) {
		return _value > value;
	}
	inline bool operator< (const T& value) {
		return _value < value;
	}
#pragma endregion
#pragma region Field Operators
	inline bool operator== (const Field<T>& field) {
		return _value == field._value;
	}
	inline bool operator!= (const Field<T>& field) {
		return _value != field._value;
	}

	inline bool operator>= (const Field<T>& field) {
		return _value >= field._value;
	}
	inline bool operator<= (const Field<T>& field) {
		return _value <= field._value;
	}
	inline bool operator> (const Field<T>& field) {
		return _value > field._value;
	}
	inline bool operator< (const Field<T>& field) {
		return _value < field._value;
	}
#pragma endregion
protected:
	T _value;
private:
	static inline std::string _fieldType;
};


class FieldFactory
{
public:
	static SField Load(nlohmann::json& input);
protected:
	template<typename T>
	static inline SField LoadField(nlohmann::json& value)
	{
		auto newField = FieldBase::Create<T>();
		newField->SetValueTemplate<T>(value.get<T>());
		return newField;
	}
	static inline std::unordered_map<std::string, SField(*)(nlohmann::json&)> _createFunction;
private:
	STATIC_CONSTRUCTOR(
		_createFunction.emplace("bool", &FieldFactory::LoadField<bool>);
		_createFunction.emplace("char", &FieldFactory::LoadField<char>);
		_createFunction.emplace("string", &FieldFactory::LoadField<std::string>);
		_createFunction.emplace("float", &FieldFactory::LoadField<float>);
		_createFunction.emplace("double", &FieldFactory::LoadField<double>);
		_createFunction.emplace("int", &FieldFactory::LoadField<int>);
		_createFunction.emplace("unsigned", &FieldFactory::LoadField<unsigned>);
		_createFunction.emplace("long long", &FieldFactory::LoadField<long long>);
		_createFunction.emplace("unsigned long long", &FieldFactory::LoadField<unsigned long long>);
	)
};

class BlackBoard 
{
public:
	static std::shared_ptr<BlackBoard> Load(nlohmann::json& input);

	template<typename T>
	inline void SetValue(const std::string& name, const T& value) {
		_fields[name]->SetValueTemplate<T>(value);
	}
	template<typename T>
	inline T GetValue(const std::string& name) {
		return _fields[name]->GetValueTemplate<T>();
	}
protected:
	template<typename T>
	inline bool AddField(const std::string& name) {
		auto result = _fields.emplace(name, FieldBase::Create<T>());
		return result.second;
	}

	template<typename T>
	inline bool AddField(const std::string& name, const T& value) {
		SField newNode = FieldBase::Create<T>();
		newNode->SetValue(value);
		auto result = _fields.emplace(name, newNode);
		return result.second;
	}
protected:
	std::unordered_map<std::string, SField> _fields;
private:
	STATIC_CONSTRUCTOR(
		Field<bool>::		_fieldType = "bool"; 
		Field<char>::		_fieldType = "char";
		Field<std::string>::_fieldType = "string";
		Field<float>::		_fieldType = "float";
		Field<double>::		_fieldType = "double";
		Field<int>::		_fieldType = "int";
		Field<unsigned>::	_fieldType = "unsigned";
		Field<long long>::	_fieldType = "long long";
		Field<unsigned long long>::_fieldType = "unsigned long long";
	)
};
typedef std::shared_ptr<BlackBoard> SBlackBoard;
typedef std::weak_ptr<BlackBoard> WBlackBoard;
