#pragma once 
#include "Engine/Math/Vec2.hpp"


struct AABB2
{
	public:
		Vec2 m_mins;
		Vec2 m_maxs;

	public:
	static const AABB2 ZERO_TO_ONE;
	AABB2() {};
	~AABB2() {};
	AABB2( AABB2 const& copyFrom );
	explicit AABB2( Vec2 const& mins, Vec2 const& maxs );
	explicit AABB2( float minX, float minY, float maxX, float maxY );

	bool IsPointInside( Vec2 const& point ) const;
	Vec2 const GetCenter() const;
	Vec2 const GetDimensions() const;
	Vec2 const GetNearestPoint( Vec2 const& referencePosition ) const;
	Vec2 const GetPointAtUV( Vec2 const& uv ) const;
	Vec2 const GetUVForPoint( Vec2 const& point ) const;

	void Translate( Vec2 const& translation );
	void SetCenter( Vec2 const& newCenter );
	void SetDimensions( Vec2 const& newDimensions );
	void SetCenterAndDimensions( Vec2 const& newCenter, Vec2 const& newDimensions);
	void StretchToIncludePoint( Vec2 const& point );

};