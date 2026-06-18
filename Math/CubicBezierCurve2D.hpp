#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/CubicHermiteCurve2D.hpp"

class CubicHermiteCurve2D;

class CubicBezierCurve2D
{
public:
	CubicBezierCurve2D() {}
	CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos);

	Vec2 EvaluateAtParametric(float t) const;
	float GetApproximateLength(int numSubdivisions = 64) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;

	explicit CubicBezierCurve2D(CubicHermiteCurve2D const& fromHermite);
public:
	Vec2 m_startPos;
	Vec2 m_guidePos1;
	Vec2 m_guidePos2;
	Vec2 m_endPos;
};