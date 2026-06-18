#pragma once

struct Vec4
{

public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	~Vec4() {}												// destructor (do nothing)
	Vec4() {}												// default constructor (do nothing)
	Vec4(Vec4 const& copyFrom);							// copy constructor (from another vec2)
	explicit Vec4(float initialX, float initialY, float initialZ, float initialW);

	bool		operator==(Vec4 const& compare) const;		// vec2 == vec2
	bool		operator!=(Vec4 const& compare) const;		// vec2 != vec2
	Vec4 const	operator+(Vec4 const& vecToAdd) const;		// vec2 + vec2
	Vec4 const	operator-(Vec4 const& vecToSubtract) const;	// vec2 - vec2
	Vec4 const	operator-() const;								// -vec2, i.e. "unary negation"
	Vec4 const	operator*(float uniformScale) const;			// vec2 * float
	Vec4 const	operator*(Vec4 const& vecToMultiply) const;	// vec2 * vec2
	Vec4 const	operator/(float inverseScale) const;			// vec2 / float

	// Operators (self-mutating / non-const)
	void		operator+=(Vec4 const& vecToAdd);				// vec2 += vec2
	void		operator-=(Vec4 const& vecToSubtract);		// vec2 -= vec2
	void		operator*=(const float uniformScale);			// vec2 *= float
	void		operator/=(const float uniformDivisor);		// vec2 /= float
	void		operator=(Vec4 const& copyFrom);				// vec2 = vec2
	
};