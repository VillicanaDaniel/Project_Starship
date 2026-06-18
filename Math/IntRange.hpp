#pragma once

struct IntRange
{
	int m_min;
	int m_max;

	IntRange();
	IntRange(int min, int max);

	IntRange& operator=(const IntRange& other);
	bool operator==(const IntRange& other) const;
	bool operator!=(const IntRange& other) const;

	bool IsOnRange(int value) const;
	bool IsOverlappingWith(const IntRange& other) const;

	static const IntRange ZERO;
	static const IntRange ONE;
	static const IntRange ZERO_TO_ONE;
};