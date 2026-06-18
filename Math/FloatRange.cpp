#include "Engine/Math/FloatRange.hpp"

FloatRange::FloatRange()
	: m_min(0.f), m_max(0.f)
{
}

FloatRange::FloatRange(float min, float max)
	: m_min(min), m_max(max)
{
}

FloatRange& FloatRange::operator=(const FloatRange& other)
{
	m_min = other.m_min;
	m_max = other.m_max;
	return *this;
}

bool FloatRange::operator==(const FloatRange& other) const
{
	return m_min == other.m_min && m_max == other.m_max;
}

bool FloatRange::operator!=(const FloatRange& other) const
{
	return !(*this == other);
}

bool FloatRange::IsOnRange(float value) const
{
	return value >= m_min && value <= m_max;
}

bool FloatRange::IsOverlappingWith(const FloatRange& other) const
{
	return m_max >= other.m_min && m_min <= other.m_max;
}

const FloatRange FloatRange::ZERO(0.f, 0.f);
const FloatRange FloatRange::ONE(1.f, 1.f);
const FloatRange FloatRange::ZERO_TO_ONE(0.f, 1.f);