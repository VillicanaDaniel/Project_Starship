#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/CubicHermiteCurve2D.hpp"
#include <vector>

class CubicHermiteSpline2D
{
public:
	CubicHermiteSpline2D() {}
	explicit CubicHermiteSpline2D(std::vector<Vec2> const& positions);

	Vec2 EvaluateAtParametric(float t) const;
	float GetApproximateLength(int numSubdivisionsPerCurve = 64) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongSpline, int numSubdivisionsPerCurve = 64) const;

	int GetNumCurves() const;

public:
	std::vector<Vec2> m_positions;
	std::vector<Vec2> m_velocities;
	std::vector<CubicHermiteCurve2D> m_curves;
};