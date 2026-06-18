#include "Engine/Core/XmlUtils.hpp"

int ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValue;
	return attr->IntValue();
}


char ParseXmlAttribute(XmlElement const& element, char const* attributeName, char defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValue;
	return static_cast<char>(attr->IntValue());
}


bool ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValue;

	return attr->BoolValue();
}


float ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValue;

	return attr->FloatValue();
}


Rgba8 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValue;

	Rgba8 result = defaultValue;
	result.SetFromText(attr->Value());
	return result;
}


Vec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValue;

	Vec2 result = defaultValue;
	result.SetFromText(attr->Value());
	return result;
}

Vec3 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec3 const& defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr)
	{
		return defaultValue;
	}

	Strings parts = SplitStringOnDelimiter(attr->Value(), ',');
	if (parts.size() != 3)
	{
		return defaultValue;
	}

	return Vec3(
		(float)atof(parts[0].c_str()),
		(float)atof(parts[1].c_str()),
		(float)atof(parts[2].c_str())
	);
}


EulerAngles ParseXmlAttribute(XmlElement const& element, char const* attributeName, EulerAngles const& defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValue;

	EulerAngles result = defaultValue;
	result.SetFromText(attr->Value());
	return result;
}


IntVec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValue;

	IntVec2 result = defaultValue;
	result.SetFromText(attr->Value());
	return result;
}


std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValue;

	return std::string(attr->Value());
}


Strings ParseXmlAttribute(XmlElement const& element, char const* attributeName, Strings const& defaultValues, char delimiter)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return defaultValues;

	return SplitStringOnDelimiter(attr->Value(), delimiter);
}


std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue)
{
	XmlAttribute const* attr = element.FindAttribute(attributeName);
	if (!attr) return std::string(defaultValue);

	return std::string(attr->Value());
}
