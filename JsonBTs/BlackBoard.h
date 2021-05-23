#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

template<typename T>
class Field;

class BlackBoard
{
public:
	class FieldBase
	{
	public:
		void SetValue(std::shared_ptr<FieldBase>& field);
	};
public:
	template<typename T>
	void AddValue(const std::string& name, const T& value);
	template<typename T>
	void SetValue(const std::string& name, const T& value);
	template<typename T>
	T& GetValue(const std::string& name);
protected:
	std::unordered_map<std::string, std::shared_ptr<BlackBoard::FieldBase>> _fields;
};
typedef std::shared_ptr<BlackBoard::FieldBase> SField;
typedef std::weak_ptr<BlackBoard::FieldBase> WField;

template<typename T>
class Field : public BlackBoard::FieldBase
{
public:
	void SetValue(const T& value);
};

template<typename T>
inline bool operator== (const SField& ptr, const T& value) {
	return dynamic_cast<T>(ptr.get()) == value;
}
template<typename T>
inline bool operator!= (const SField& ptr, const T& value) {
	return dynamic_cast<T>(ptr.get()) != value;
}
template<typename T>
inline bool operator>  (const SField& ptr, const T& value) {
	return dynamic_cast<T>(ptr.get()) > value;
}
template<typename T>
inline bool operator<  (const SField& ptr, const T& value) {
	return dynamic_cast<T>(ptr.get()) < value;
}
template<typename T>
inline bool operator>= (const SField& ptr, const T& value) {
	return dynamic_cast<T>(ptr.get()) >= value;
}
template<typename T>
inline bool operator<= (const SField& ptr, const T& value) {
	return dynamic_cast<T>(ptr.get()) <= value;
}

template<typename T>
inline void Field<T>::SetValue(const T& value)
{
	*this = value;
}

template<typename T>
inline void BlackBoard::AddValue(const std::string& name, const T& value)
{
	if (auto result = _fields.emplace(name, std::make_shared<Field<T>>(value)); !result.second)
	{
		throw std::exception();
	}
}

template<typename T>
inline void BlackBoard::SetValue(const std::string& name, const T& value)
{
	dynamic_cast<Field<T>>(_fields[name])->SetValue(value);
}

template<typename T>
inline T& BlackBoard::GetValue(const std::string& name)
{
	return _fields[name];
}
