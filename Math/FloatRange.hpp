#pragma once

struct FloatRange
{
	float m_min = 0.f;
	float m_max = 0.f;

	FloatRange();
	FloatRange(float min, float max);

	FloatRange& operator=(const FloatRange& other);
	bool operator==(const FloatRange& other) const;
	bool operator!=(const FloatRange& other) const;

	bool IsOnRange(float value) const;
	bool IsOverlappingWith(const FloatRange& other) const;

	static const FloatRange ZERO;
	static const FloatRange ONE;
	static const FloatRange ZERO_TO_ONE;
};