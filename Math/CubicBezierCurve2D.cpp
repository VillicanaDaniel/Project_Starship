#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"

CubicBezierCurve2D::CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos)
	: m_startPos(startPos)
	, m_guidePos1(guidePos1)
	, m_guidePos2(guidePos2)
	, m_endPos(endPos)
{
}

CubicBezierCurve2D::CubicBezierCurve2D(CubicHermiteCurve2D const& fromHermite)
{
	m_startPos = fromHermite.m_startPos;
	m_guidePos1 = fromHermite.m_startPos + fromHermite.m_startVelocity * (1.f / 3.f);
	m_guidePos2 = fromHermite.m_endPos - fromHermite.m_endVelocity * (1.f / 3.f);
	m_endPos = fromHermite.m_endPos;
}

Vec2 CubicBezierCurve2D::EvaluateAtParametric(float t) const
{
	Vec2 AB = m_startPos + (m_guidePos1 - m_startPos) * t;
	Vec2 BC = m_guidePos1 + (m_guidePos2 - m_guidePos1) * t;
	Vec2 CD = m_guidePos2 + (m_endPos - m_guidePos2) * t;

	Vec2 ABC = AB + (BC - AB) * t;
	Vec2 BCD = BC + (CD - BC) * t;

	Vec2 ABCD = ABC + (BCD - ABC) * t;

	return ABCD;
}

float CubicBezierCurve2D::GetApproximateLength(int numSubdivisions) const
{
	float length = 0.f;

	Vec2 previousPoint = EvaluateAtParametric(0.f);

	for (int i = 1; i <= numSubdivisions; ++i)
	{
		float t = (float)i / (float)numSubdivisions;
		Vec2 point = EvaluateAtParametric(t);

		length += GetDistance2D(previousPoint, point);
		previousPoint = point;
	}

	return length;
}

Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions) const
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
			float distanceIntoSegment = distanceAlongCurve - distanceSoFar;
			float fraction = distanceIntoSegment / segmentLength;

			return previousPoint + (currentPoint - previousPoint) * fraction;
		}

		distanceSoFar += segmentLength;
		previousPoint = currentPoint;
	}

	return m_endPos;
}