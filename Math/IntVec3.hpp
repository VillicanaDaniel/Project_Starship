#pragma once

struct IntVec3
{
public:
	IntVec3() = default;
	IntVec3(int initialX, int initialY, int initialZ);

	bool operator==(IntVec3 const& compare) const;
	bool operator!=(IntVec3 const& compare) const;

	IntVec3 operator+(IntVec3 const& vecToAdd) const;
	IntVec3 operator-(IntVec3 const& vecToSubtract) const;

public:
	int x = 0;
	int y = 0;
	int z = 0;

	static const IntVec3 ZERO;
};