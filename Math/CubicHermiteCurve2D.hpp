#pragma once
#include "Engine/Math/Vec2.hpp"

class CubicBezierCurve2D;

class CubicHermiteCurve2D
{
public:
	CubicHermiteCurve2D() {}
	CubicHermiteCurve2D(Vec2 startPos, Vec2 startVelocity, Vec2 endPos, Vec2 endVelocity);
	explicit CubicHermiteCurve2D(CubicBezierCurve2D const& fromBezier);

	Vec2 EvaluateAtParametric(float t) const;
	float GetApproximateLength(int numSubdivisions = 64) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;

public:
	Vec2 m_startPos;
	Vec2 m_startVelocity;
	Vec2 m_endPos;
	Vec2 m_endVelocity;
};