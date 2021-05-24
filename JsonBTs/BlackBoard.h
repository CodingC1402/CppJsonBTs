#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "Macros.h"
#include "ObservePattern.h"
#include "json.hpp"

class FieldFactory;
class FieldBase;
typedef std::weak_ptr<FieldBase> WField;
typedef std::shared_ptr<FieldBase> SField;

template<typename T>
class Field;

class FieldBase : public Observable
{
public:
	inline std::string GetFieldName() {
		return _fieldName;
	}
	inline std::string GetFieldType() {
		return _fieldType;
	}

	template<typename T>
	static inline std::shared_ptr<FieldBase> Create(const std::string& name) {
		std::shared_ptr<FieldBase> newField;
		newField.reset(new Field<T>());
		newField->_fieldName = name;
		newField->AssignPtr(newField);
		return newField;
	}
	template<typename T>
	static inline std::shared_ptr<FieldBase> Create(const std::string& name, const T& value) {
		std::shared_ptr<FieldBase> newField;
		newField.reset(new Field<T>());
		newField->_fieldName = name;
		newField->AssignPtr(newField);
		newField->SetValueTemplate<T>(value);
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
	void AssignPtr(const std::weak_ptr<FieldBase>& ptr);
protected:
	static inline std::string _fieldType;
	std::weak_ptr<FieldBase> _this;
	std::string _fieldName;
};

#pragma region Templates
template<typename T>
class Field : public FieldBase
{
	friend class BlackBoard;
public:
	inline void SetValue(const T& value) {
		_value = value;
		CallOnChange();
	}
	inline T GetValue() {
		return _value;
	}
	inline SField Clone() {
		return FieldBase::Create<T>(_fieldName, _value);
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

	static inline void SetTypeName(const std::string& name) {
		if (_setOnce)
		{
			//Change later
			throw std::exception();
		}
		_fieldType = name;
		_setOnce = true;
	}
protected:
	T _value;
private:
	static inline bool _setOnce = false;
};
#pragma endregion;

#define FIELD_REGISTER(type) FieldFactory::AddFieldType<type>(#type); Field<type>::SetTypeName(#type)
class FieldFactory
{
public:
	template<typename T>
	static inline void AddFieldType(const std::string& name) {
		_createFunction.emplace(name, &FieldFactory::LoadField<T>);
		_cloneFunction.emplace(name, &FieldFactory::CloneField<T>);
	}

	static SField Load(nlohmann::json& input);
	static SField Clone(WField toClone);
protected:
	template<typename T>
	static inline SField LoadField(nlohmann::json& value)
	{
		auto newField = FieldBase::Create<T>(value[BTField::nameField].get<std::string>());
		newField->SetValueTemplate<T>(value[BTField::valueField].get<T>());
		return newField;
	}
	template<typename T>
	static inline SField CloneField(WField toClone)
	{
		return std::dynamic_pointer_cast<Field<T>>(toClone.lock())->Clone();
	}
protected:
	static inline std::unordered_map<std::string, SField(*)(nlohmann::json&)> _createFunction;
	static inline std::unordered_map<std::string, SField(*)(WField)> _cloneFunction;
private:
	STATIC_CONSTRUCTOR(
		using string = std::string;
		FIELD_REGISTER(bool);
		FIELD_REGISTER(char);
		FIELD_REGISTER(string);
		FIELD_REGISTER(float);
		FIELD_REGISTER(double);
		FIELD_REGISTER(unsigned);
		FIELD_REGISTER(int);
		FIELD_REGISTER(long long);
		FIELD_REGISTER(unsigned long long);
	)
};

class BlackBoard;
typedef std::shared_ptr<BlackBoard> SBlackBoard;
typedef std::weak_ptr<BlackBoard> WBlackBoard;
class BlackBoard 
{
public:
	static std::shared_ptr<BlackBoard> Load(nlohmann::json& input);
	SBlackBoard Clone();

	template<typename T>
	inline void SetValue(const std::string& name, const T& value) {
		_fields[name]->SetValueTemplate<T>(value);
	}
	template<typename T>
	inline T GetValue(const std::string& name) {
		return _fields[name]->GetValueTemplate<T>();
	}
	template<typename T>
	inline std::weak_ptr<Field<T>> GetField(const std::string& name) {
		return std::dynamic_pointer_cast<Field<T>>(_fields[name]);
	}
protected:
	template<typename T>
	inline bool AddField(const std::string& name) {
		auto result = _fields.emplace(name, FieldBase::Create<T>(name));
		return result.second;
	}

	template<typename T>
	inline bool AddField(const std::string& name, const T& value) {
		SField newNode = FieldBase::Create<T>(name);
		newNode->SetValue(value);
		auto result = _fields.emplace(name, newNode);
		return result.second;
	}
protected:
	std::unordered_map<std::string, SField> _fields;
};
