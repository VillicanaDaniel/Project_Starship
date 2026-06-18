#include "Engine/Math/CubicHermiteCurve2D.hpp"
#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"

CubicHermiteCurve2D::CubicHermiteCurve2D(
	Vec2 startPos,
	Vec2 startVelocity,
	Vec2 endPos,
	Vec2 endVelocity)
	: m_startPos(startPos)
	, m_startVelocity(startVelocity)
	, m_endPos(endPos)
	, m_endVelocity(endVelocity)
{
}

CubicHermiteCurve2D::CubicHermiteCurve2D(CubicBezierCurve2D const& fromBezier)
{
	m_startPos = fromBezier.m_startPos;
	m_endPos = fromBezier.m_endPos;

	m_startVelocity = (fromBezier.m_guidePos1 - fromBezier.m_startPos) * 3.f;
	m_endVelocity = (fromBezier.m_endPos - fromBezier.m_guidePos2) * 3.f;
}

Vec2 CubicHermiteCurve2D::EvaluateAtParametric(float t) const
{
	float t2 = t * t;
	float t3 = t2 * t;

	float h00 = 2.f * t3 - 3.f * t2 + 1.f;
	float h10 = t3 - 2.f * t2 + t;
	float h01 = -2.f * t3 + 3.f * t2;
	float h11 = t3 - t2;

	return
		m_startPos * h00 +
		m_startVelocity * h10 +
		m_endPos * h01 +
		m_endVelocity * h11;
}

float CubicHermiteCurve2D::GetApproximateLength(int numSubdivisions) const
{
	float length = 0.f;
	Vec2 previousPoint = EvaluateAtParametric(0.f);

	for (int i = 1; i <= numSubdivisions; ++i)
	{
		float t = (float)i / (float)numSubdivisions;
		Vec2 currentPoint = EvaluateAtParametric(t);

		length += GetDistance2D(previousPoint, currentPoint);
		previousPoint = currentPoint;
	}

	return length;
}

Vec2 CubicHermiteCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions) const
{
	if (distanceAlongCurve <= 0.f)
	{
		return m_startPos;
	}

	Vec2 previousPoint = EvaluateAtParametric(0.f);
	float distanceSoFar = 0.f;

	for (int i = 1; i <= numSubdivisions; ++i)
	{
		float t = (float)i / (float)numSubdivisions;
		Vec2 currentPoint = EvaluateAtParametric(t);

		float segmentLength = GetDistance2D(previousPoint, currentPoint);

		if (distanceSoFar + segmentLength >= distanceAlongCurve)
		{
			float fraction = (distanceAlongCurve - distanceSoFar) / segmentLength;
			return previousPoint + (currentPoint - previousPoint) * fraction;
		}

		distanceSoFar += segmentLength;
		previousPoint = currentPoint;
	}

	return m_endPos;
}