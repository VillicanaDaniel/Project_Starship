#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <cmath>

const Vec3 Vec3::ZERO = Vec3(0.f, 0.f, 0.f);

Vec3::Vec3( Vec3 const& copy )
	: x(copy.x)
	, y(copy.y)
	, z(copy.z)
{
}

Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}


void Vec3::SetFromText(char const* text)
{
	if (text == nullptr)
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
		return;
	}

	Strings parts = SplitStringOnDelimiter(text, ',');

	if (parts.size() != 3)
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
		return;
	}

	x = (float)atof(parts[0].c_str());
	y = (float)atof(parts[1].c_str());
	z = (float)atof(parts[2].c_str());
}


Vec3 const Vec3::operator+ (Vec3 const& vecToAdd) const
{
	return Vec3(this->x + vecToAdd.x, this->y + vecToAdd.y, this->z + vecToAdd.z);
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-(Vec3 const& vecToSubtract) const
{
	return Vec3(this->x - vecToSubtract.x, this->y - vecToSubtract.y, this->z - vecToSubtract.z);
}


//------------------------------------------------------------------------------------------------
Vec3 const Vec3::operator-() const
{
	return Vec3(0 - this->x, 0 - this->y, 0 - this->z);
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*(float uniformScale) const
{
	return Vec3(this->x * uniformScale, this->y * uniformScale, this->z * uniformScale);
}


//------------------------------------------------------------------------------------------------
Vec3 const Vec3::operator*(Vec3 const& vecToMultiply) const
{
	return Vec3(this->x * vecToMultiply.x, this->y * vecToMultiply.y, this->z * vecToMultiply.z);
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::operator/(float inverseScale) const
{
	return Vec3(this->x / inverseScale, this->y / inverseScale, this->z / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=(Vec3 const& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=(Vec3 const& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=(const float uniformDivisor)
{
	this->x /= uniformDivisor;
	this->y /= uniformDivisor;
	this->z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=(Vec3 const& copyFrom)
{
	this->x = copyFrom.x;
	this->y = copyFrom.y;
	this->z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
Vec3 const operator*(float uniformScale, Vec3 const& vecToScale)
{
	return Vec3(uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==(Vec3 const& compare) const
{
	return this->x == compare.x && this->y == compare.y && this->z == compare.z;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=(Vec3 const& compare) const
{
	return this->x != compare.x || this->y != compare.y || this->z != compare.z;
}

float Vec3::GetLength() const
{
	return sqrt(GetLengthSquared());
}

float Vec3::GetLengthXY() const
{
	return sqrt(GetLengthXYSquared());
}

float Vec3::GetLengthSquared() const
{
	return ( x * x + y * y + z * z );
}

float Vec3::GetLengthXYSquared() const
{
	return ( x * x + y * y );
}
float Vec3::GetOrientationAboutZDegrees() const
{
	float angle = Atan2Degrees(y, x);
	return angle;

	//Might need to correct for 360 degrees
}
float Vec3::GetOrientationAboutZRadians() const
{
	float angle = Atan2Degrees(y, x);
	angle = ConvertDegreesToRadians(angle);
	return angle;
}
Vec3 const Vec3::GetRotatedAboutZDegrees(float rotationDegreesAboutZ) const
{
	float angle_degrees = GetOrientationAboutZDegrees();
	float radius = GetLengthXY();

	angle_degrees += rotationDegreesAboutZ;

	float temp_x = radius * CosDegrees(angle_degrees);
	float temp_y = radius * SinDegrees(angle_degrees);

	return Vec3(temp_x, temp_y, z);
}

Vec3 const Vec3::GetRotatedAboutZRadians(float rotationRadiansAboutZ) const
{
	float angle_radians = GetOrientationAboutZRadians();
	float radius = GetLengthXY();

	angle_radians += rotationRadiansAboutZ;

	float temp_x = radius * cosf(angle_radians);
	float temp_y = radius * sinf(angle_radians);

	return Vec3(temp_x, temp_y, z);
}

Vec3 const Vec3::GetClampedToMaxLength(float MaxLength) const
{
	float length = GetLength();

	if (length <= MaxLength)
	{
		return Vec3(x, y, z);
	}

	float scale = MaxLength / length;

	return Vec3(x * scale, y * scale, z * scale);
}

Vec3 const Vec3::GetNormalized() const
{
	float temp_x = x / GetLength();
	float temp_y = y / GetLength();
	float temp_z = z / GetLength();
	return Vec3( temp_x, temp_y, temp_z );
}