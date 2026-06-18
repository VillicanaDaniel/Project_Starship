#pragma once 
#include "Engine/Math/Vec2.hpp"


struct Vec3
{
public: 
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

public:
	// Construction/Destruction
	~Vec3() {}												// destructor (do nothing)
	Vec3() {}												// default constructor (do nothing)
	Vec3(Vec3 const& copyFrom);							// copy constructor (from another vec2)
	explicit Vec3(float initialX, float initialY, float initialZ);
	void SetFromText(char const* text);

	static const Vec3 ZERO;
	//Accessors
	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	float GetOrientationAboutZDegrees() const;
	float GetOrientationAboutZRadians() const;
	Vec3 const GetRotatedAboutZDegrees( float rotationDegreesAboutZ ) const;
	Vec3 const GetRotatedAboutZRadians( float rotationRadiansAboutZ ) const;
	Vec3 const GetClampedToMaxLength( float MaxLength ) const;
	Vec3 const GetNormalized() const;

	bool		operator==(Vec3 const& compare) const;		// vec2 == vec2
	bool		operator!=(Vec3 const& compare) const;		// vec2 != vec2
	Vec3 const	operator+(Vec3 const& vecToAdd) const;		// vec2 + vec2
	Vec3 const	operator-(Vec3 const& vecToSubtract) const;	// vec2 - vec2
	Vec3 const	operator-() const;								// -vec2, i.e. "unary negation"
	Vec3 const	operator*(float uniformScale) const;			// vec2 * float
	Vec3 const	operator*(Vec3 const& vecToMultiply) const;	// vec2 * vec2
	Vec3 const	operator/(float inverseScale) const;			// vec2 / float

	// Operators (self-mutating / non-const)
	void		operator+=(Vec3 const& vecToAdd);				// vec2 += vec2
	void		operator-=(Vec3 const& vecToSubtract);		// vec2 -= vec2
	void		operator*=(const float uniformScale);			// vec2 *= float
	void		operator/=(const float uniformDivisor);		// vec2 /= float
	void		operator=(Vec3 const& copyFrom);				// vec2 = vec2

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
	friend Vec3 const operator*(float uniformScale, Vec3 const& vecToScale);	// float * vec2


};