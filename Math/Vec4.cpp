#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>

Vec4::Vec4(Vec4 const& copy)
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
	, w(copy.w)
{
}

Vec4::Vec4(float initialX, float initialY, float initialZ, float initialW)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
	, w(initialW)
{
}

Vec4 const Vec4::operator+ (Vec4 const& vecToAdd) const
{
	return Vec4(this->x + vecToAdd.x, this->y + vecToAdd.y, this->z + vecToAdd.z, this->w + vecToAdd.w);
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator-(Vec4 const& vecToSubtract) const
{
	return Vec4(this->x - vecToSubtract.x, this->y - vecToSubtract.y, this->z - vecToSubtract.z, this->w - vecToSubtract.w);
}


//------------------------------------------------------------------------------------------------
Vec4 const Vec4::operator-() const
{
	return Vec4(0 - this->x, 0 - this->y, 0 - this->z, 0 - this->w);
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator*(float uniformScale) const
{
	return Vec4(this->x * uniformScale, this->y * uniformScale, this->z * uniformScale, this->w * uniformScale);
}


//------------------------------------------------------------------------------------------------
Vec4 const Vec4::operator*(Vec4 const& vecToMultiply) const
{
	return Vec4(this->x * vecToMultiply.x, this->y * vecToMultiply.y, this->z * vecToMultiply.z, this->w * vecToMultiply.w);
}


//-----------------------------------------------------------------------------------------------
Vec4 const Vec4::operator/(float inverseScale) const
{
	return Vec4(this->x / inverseScale, this->y / inverseScale, this->z / inverseScale, this->w / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator+=(Vec4 const& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator-=(Vec4 const& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator/=(const float uniformDivisor)
{
	this->x /= uniformDivisor;
	this->y /= uniformDivisor;
	this->z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=(Vec4 const& copyFrom)
{
	this->x = copyFrom.x;
	this->y = copyFrom.y;
	this->z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
Vec4 const operator*(float uniformScale, Vec4 const& vecToScale)
{
	return Vec4(uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z, uniformScale * vecToScale.w);
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==(Vec4 const& compare) const
{
	return this->x == compare.x && this->y == compare.y && this->z == compare.z && this->w == compare.w;
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator!=(Vec4 const& compare) const
{
	return this->x != compare.x || this->y != compare.y || this->z != compare.z || this->w != compare.w;
}