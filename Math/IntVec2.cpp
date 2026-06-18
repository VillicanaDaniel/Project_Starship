#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>

const IntVec2 IntVec2::ZERO = IntVec2(0, 0);

IntVec2::IntVec2(const IntVec2& copyFrom)
:x( copyFrom.x )
,y( copyFrom.y )
{
}

IntVec2::IntVec2(int initialX, int initialY)
:x( initialX )
,y( initialY )
{
}

void IntVec2::SetFromText(char const* text)
{
	Strings parts = SplitStringOnDelimiter(text, ',');

	if (parts.size() != 2)
	{
		ERROR_AND_DIE("IntVec2::SetFromText() - Expected exactly one comma.");
	}

	x = atoi(parts[0].c_str());
	y = atoi(parts[1].c_str());
}

float IntVec2::GetLength() const
{
	return sqrt( static_cast<float>(GetLengthSquared()) );
}

int IntVec2::GetTaxicabLength() const
{
	return abs(x) + abs(y);
}

int IntVec2::GetLengthSquared() const
{
	return (x * x + y * y);
}

float IntVec2::GetOrientationRadians() const
{
	float angle = Atan2Degrees(static_cast<float>(y), static_cast<float>(x));
	angle = ConvertDegreesToRadians(angle);
	return angle;
}

float IntVec2::GetOrientationDegrees() const
{
	float angle = Atan2Degrees(static_cast<float>(y), static_cast<float>(x));
	return angle;
}

IntVec2 const IntVec2::GetRotatedBy90Degrees() const
{
	return IntVec2( -y, x);
}

IntVec2 const IntVec2::GetRotatedByMinus90Degrees() const
{
	return IntVec2( y, -x);
}

void IntVec2::Rotate90Degrees()
{
	int t = x;
	x = -y;
	y = t;
}

void IntVec2::RotateMinus90Degrees()
{
	int t = x;
	x = y;
	y = -t;
}

void IntVec2::operator=(const IntVec2& copyFrom)
{
x = copyFrom.x;
y = copyFrom.y;
}

IntVec2 const IntVec2::operator+(IntVec2 const& vecToAdd) const
{
	return IntVec2(this->x + vecToAdd.x, this->y + vecToAdd.y);
}

IntVec2 const IntVec2::operator-(IntVec2 const& vecToSubtract) const
{
	return IntVec2(this->x - vecToSubtract.x, this->y - vecToSubtract.y);
}
