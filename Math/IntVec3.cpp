#include "Engine/Math/IntVec3.hpp"

const IntVec3 IntVec3::ZERO = IntVec3(0, 0, 0);

IntVec3::IntVec3(int initialX, int initialY, int initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}

bool IntVec3::operator==(IntVec3 const& compare) const
{
	return x == compare.x
		&& y == compare.y
		&& z == compare.z;
}

bool IntVec3::operator!=(IntVec3 const& compare) const
{
	return !(*this == compare);
}

IntVec3 IntVec3::operator+(IntVec3 const& vecToAdd) const
{
	return IntVec3(
		x + vecToAdd.x,
		y + vecToAdd.y,
		z + vecToAdd.z
	);
}

IntVec3 IntVec3::operator-(IntVec3 const& vecToSubtract) const
{
	return IntVec3(
		x - vecToSubtract.x,
		y - vecToSubtract.y,
		z - vecToSubtract.z
	);
}