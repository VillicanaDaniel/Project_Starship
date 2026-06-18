#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>

const Vec2 Vec2::ZERO = Vec2(0.f, 0.f);

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( Vec2 const& copy )
	: x( copy.x)
	, y( copy.y)
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX)
	, y( initialY )
{
}

void Vec2::SetFromText(char const* text)
{
	Strings parts = SplitStringOnDelimiter(text, ',');

	// should only have x and y?
	if (parts.size() != 2)
	{
		ERROR_AND_DIE("Vec2::SetFromText() - Invalid format, expected exactly one comma.");
	}

	x = static_cast<float>(atof(parts[0].c_str()));
	y = static_cast<float>(atof(parts[1].c_str()));
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator+ ( Vec2 const& vecToAdd ) const
{
	return Vec2(this->x + vecToAdd.x, this->y + vecToAdd.y);
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-( Vec2 const& vecToSubtract ) const
{
	return Vec2(this->x - vecToSubtract.x, this->y - vecToSubtract.y);
}


//------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-() const
{
	return Vec2( 0-this->x,0-this->y);
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( float uniformScale ) const
{
	return Vec2( this->x * uniformScale, this->y * uniformScale );
}


//------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( Vec2 const& vecToMultiply ) const
{
	return Vec2( this->x * vecToMultiply.x, this->y * vecToMultiply.y );
}

//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::operator/( float inverseScale ) const
{
	return Vec2( this->x / inverseScale, this->y / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( Vec2 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( Vec2 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	this->x /= uniformDivisor;
	this->y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( Vec2 const& copyFrom )
{
	this->x = copyFrom.x;
	this->y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
Vec2 const operator*( float uniformScale, Vec2 const& vecToScale )
{
	return Vec2( uniformScale * vecToScale.x, uniformScale * vecToScale.y );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( Vec2 const& compare ) const
{
	return this->x == compare.x && this->y == compare.y;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( Vec2 const& compare ) const
{
	return this->x != compare.x || this->y != compare.y;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Special Constructor
Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegrees, float length)
{ 
	float temp_x = CosDegrees(orientationDegrees) * length;
	float temp_y = SinDegrees(orientationDegrees) * length;

	return Vec2(temp_x, temp_y);
}

Vec2 const Vec2::MakeFromPolarRadians(float orientationRadians, float length)
{
	float temp_x = cosf(orientationRadians) * length;
	float temp_y = sinf(orientationRadians) * length;

	return Vec2(temp_x, temp_y);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Accessors
float Vec2::GetLength() const
{
	return sqrt(GetLengthSquared());
}

float Vec2::GetLengthSquared() const
{
	return (x * x + y * y);
}

float Vec2::GetOrientationDegrees() const
{
	float angle = Atan2Degrees( y, x );
	return angle;
	//Might need to correct for 360 degrees
}

float Vec2::GetOrientationRadians() const
{
	float angle = Atan2Degrees( y, x );
	angle = ConvertDegreesToRadians(angle);
	return angle;
}

Vec2 const Vec2::GetRotatedBy90Degrees() const
{
	float angle_degrees = GetOrientationDegrees();
	float radius = GetLength();

	angle_degrees += 90;

	float temp_x = radius * CosDegrees(angle_degrees);
	float temp_y = radius * SinDegrees(angle_degrees);

	return Vec2( temp_x, temp_y );
}
Vec2 const Vec2::GetRotatedByMinus90Degrees() const 
{
	float angle_degrees = GetOrientationDegrees();
	float radius = GetLength();

	angle_degrees -= 90;

	float temp_x = radius * CosDegrees(angle_degrees);
	float temp_y = radius * SinDegrees(angle_degrees);

	return Vec2(temp_x, temp_y);
}

Vec2 const Vec2::GetRotatedByDegrees(float rotationDegrees) const
{
	float angle_degrees = GetOrientationDegrees();
	float radius = GetLength();

	angle_degrees += rotationDegrees;

	float temp_x = radius * CosDegrees(angle_degrees);
	float temp_y = radius * SinDegrees(angle_degrees);

	return Vec2(temp_x, temp_y);
}

Vec2 const Vec2::GetRotatedByRadians(float rotationRadians) const
{
	float angle_radians = GetOrientationRadians();
	float radius = GetLength();

	angle_radians += rotationRadians;

	float temp_x = radius * cosf(angle_radians);
	float temp_y = radius * sinf(angle_radians);

	return Vec2(temp_x, temp_y);
}

Vec2 const Vec2::GetClamped(float maxLength) const
{
	float length = GetLength();

	if ( length <= maxLength ) 
	{
		return Vec2( x , y); 
	}

	float scale = maxLength / length;

	return Vec2(x * scale, y * scale);
}

Vec2 const Vec2::GetNormalized() const
{
	float temp_x = x / GetLength();
	float temp_y = y / GetLength();
	return Vec2( temp_x , temp_y );
}

Vec2 const Vec2::GetReflected(Vec2 const& normalOfSurfaceToReflectOffOf) const
{
	float dot = DotProduct2D(*this, normalOfSurfaceToReflectOffOf);
	return *this - 2.f * dot * normalOfSurfaceToReflectOffOf;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Mutators
void Vec2::SetOrientationDegrees(float newOrientationDegrees)
{
	float length = GetLength();

	x = CosDegrees(newOrientationDegrees) * length;
	y = SinDegrees(newOrientationDegrees) * length;

}

void Vec2::SetOrientationRadians(float newOrientationRadians)
{
	float length = GetLength();

	x = cosf(newOrientationRadians) * length;
	y = sinf(newOrientationRadians) * length;
}

void Vec2::SetPolarDegrees(float newOrientationDegrees, float newLength)
{
	x = CosDegrees(newOrientationDegrees) * newLength;
	y = SinDegrees(newOrientationDegrees) * newLength;
}

void Vec2::SetPolarRadians(float newOrientationRadians, float newLength)
{
	x = cosf(newOrientationRadians) * newLength;
	y = sinf(newOrientationRadians) * newLength;
}

void Vec2::Rotate90Degrees()
{
	float angle_degrees = GetOrientationDegrees();
	float radius = GetLength();

	angle_degrees += 90;

	x = radius * CosDegrees(angle_degrees);
	y = radius * SinDegrees(angle_degrees);
}

void Vec2::RotateMinus90Degrees()
{
	float angle_degrees = GetOrientationDegrees();
	float radius = GetLength();

	angle_degrees -= 90;

	x = radius * CosDegrees(angle_degrees);
	y = radius * SinDegrees(angle_degrees);
}

void Vec2::RotateDegrees(float rotationDegrees)
{
	float angle_degrees = GetOrientationDegrees();
	float radius = GetLength();

	angle_degrees += rotationDegrees;

	x = radius * CosDegrees(angle_degrees);
	y = radius * SinDegrees(angle_degrees);
}

void Vec2::RotateRadians(float rotationRadians)
{
	float angle_radians = GetOrientationRadians();
	float radius = GetLength();

	angle_radians += rotationRadians;

	x = radius * cosf(angle_radians);
	y = radius * sinf(angle_radians);
}

void Vec2::SetLength(float newLength)
{
	float currentLength = GetLength();

	float scale = newLength / currentLength;
	x *= scale;
	y *= scale;
}

void Vec2::ClampLength(float maxLength)
{
	float length = GetLength();

	if (length <= maxLength)
	{
		return;
	}

	float scale = maxLength / length;

	x *= scale;
	y *= scale;
}

void Vec2::Normalize()
{
	float length = GetLength();
	if (length == 0.0f) {
		return; 
	}

	x /= length;
	y /= length;
}

float Vec2::NormalizeAndGetPreviousLength()
{
	float previous_length = GetLength();

	x = x / previous_length;
	y = y / previous_length;

	return previous_length;
}

void Vec2::Reflect(Vec2 const& normalOfSurfaceToReflectOffOf)
{
	float dot = DotProduct2D(*this, normalOfSurfaceToReflectOffOf);
	*this = *this - 2.f * dot * normalOfSurfaceToReflectOffOf;
}

