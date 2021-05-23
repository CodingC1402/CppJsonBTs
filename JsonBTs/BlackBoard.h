#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Macros.h"

template<typename T>
class Field;

class FieldBase
{
public:
	template<typename T>
	std::shared_ptr<FieldBase> Create() {
		std::shared_ptr<FieldBase> newNode(new Field<T>());
		newNode->AssignPtr(newNode);
		return newNode;
	}

	void AssignPtr(const std::weak_ptr<FieldBase>& ptr);
	template<typename T>
	inline void SetValue(const T& value) {
		dynamic_cast<Field<T>*>(this)->SetValue(value);
	}
	template<typename T>
	inline T GetValue() {
		return dynamic_cast<Field<T>*>(this)->GetValue();
	}
protected:
	std::weak_ptr<FieldBase> _this;
};
typedef std::weak_ptr<FieldBase> WField;
typedef std::shared_ptr<FieldBase> SField;

template<typename T>
class Field : FieldBase
{
	friend class BlackBoard;
public:
	inline void SetField(const T& value) {
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

class BlackBoard 
{
public:
	template<typename T>
	inline void SetValue(const std::string& name, const T& value) {
		_fields[name]->SetValue<T>(value);
	}
	template<typename T>
	inline bool GetValue(const std::string& name) {
		return _fields[name]->GetValue<T>();
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
		Field<bool>::_fieldType = "bool";
		Field<char>::_fieldType = "char";
		Field<std::string>::_fieldType = "string";
		Field<int>::_fieldType = "int";
		Field<long long>::_fieldType = "long long";
		Field<float>::_fieldType = "float";
		Field<double>::_fieldType = "double";
	)
};
