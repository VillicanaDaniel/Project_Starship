#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/StringUtils.hpp"

void NamedStrings::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	for (auto attr = element.FirstAttribute(); attr != nullptr; attr = attr->Next())
	{
		std::string key = attr->Name();
		std::string value = attr->Value();
		m_keyValuePairs[key] = value;
	}
}

void NamedStrings::SetValue(std::string const& keyName, std::string const& newValue)
{
	m_keyValuePairs[keyName] = newValue;
}

//-------------------------------------------------------------
// STRING
//-------------------------------------------------------------
std::string NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	auto iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end())
		return defaultValue;

	return iter->second;
}

//-------------------------------------------------------------
// BOOL
//-------------------------------------------------------------
bool NamedStrings::GetValue(std::string const& keyName, bool defaultValue) const
{
	auto iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end()) return defaultValue;

	std::string v = iter->second;
	return (v == "true" || v == "1");
}

//-------------------------------------------------------------
// INT
//-------------------------------------------------------------
int NamedStrings::GetValue(std::string const& keyName, int defaultValue) const
{
	auto iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end()) return defaultValue;

	return std::stoi(iter->second);
}

//-------------------------------------------------------------
// FLOAT
//-------------------------------------------------------------
float NamedStrings::GetValue(std::string const& keyName, float defaultValue) const
{
	auto iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end()) return defaultValue;

	return std::stof(iter->second);
}

//-------------------------------------------------------------
// RGBA8
//-------------------------------------------------------------
Rgba8 NamedStrings::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const
{
	auto iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end()) return defaultValue;

	Rgba8 c;
	c.SetFromText(iter->second.c_str());
	return c;
}

//-------------------------------------------------------------
// VEC2
//-------------------------------------------------------------
Vec2 NamedStrings::GetValue(std::string const& keyName, Vec2 const& defaultValue) const
{
	auto iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end()) return defaultValue;

	Vec2 v;
	v.SetFromText(iter->second.c_str());
	return v;
}

//-------------------------------------------------------------
// INTVEC2
//-------------------------------------------------------------
IntVec2 NamedStrings::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const
{
	auto iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end()) return defaultValue;

	IntVec2 v;
	v.SetFromText(iter->second.c_str());
	return v;
}

std::string NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const
{
	auto iter = m_keyValuePairs.find(keyName);
	if (iter == m_keyValuePairs.end())
	{
		return (defaultValue != nullptr) ? std::string(defaultValue) : std::string();
	}

	return iter->second;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void NamedStrings::SetValue(std::string const& key, unsigned long long value)
{
	m_keyValuePairs[key] = std::to_string(value);
}

unsigned long long NamedStrings::GetValue(std::string const& key, unsigned long long defaultValue) const
{
	auto found = m_keyValuePairs.find(key);
	if (found == m_keyValuePairs.end())
		return defaultValue;

	return std::stoull(found->second);
}