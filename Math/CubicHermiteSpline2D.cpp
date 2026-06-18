#include "Engine/Math/CubicHermiteSpline2D.hpp"
#include "Engine/Math/MathUtils.hpp"

CubicHermiteSpline2D::CubicHermiteSpline2D(std::vector<Vec2> const& positions)
{
	m_positions = positions;
	m_velocities.clear();
	m_curves.clear();

	if (m_positions.size() < 2)
	{
		return;
	}

	m_velocities.resize(m_positions.size(), Vec2::ZERO);

	for (int i = 0; i < (int)m_positions.size(); ++i)
	{
		if (i == 0 || i == (int)m_positions.size() - 1)
		{
			m_velocities[i] = Vec2::ZERO;
		}
		else
		{
			m_velocities[i] = (m_positions[i + 1] - m_positions[i - 1]) * 0.5f;
		}
	}

	for (int i = 0; i < (int)m_positions.size() - 1; ++i)
	{
		m_curves.push_back(CubicHermiteCurve2D(
			m_positions[i],
			m_velocities[i],
			m_positions[i + 1],
			m_velocities[i + 1]
		));
	}
}

int CubicHermiteSpline2D::GetNumCurves() const
{
	return (int)m_curves.size();
}

Vec2 CubicHermiteSpline2D::EvaluateAtParametric(float t) const
{
	if (m_curves.empty())
	{
		return Vec2::ZERO;
	}

	float totalT = t * (float)m_curves.size();
	int curveIndex = (int)totalT;

	if (curveIndex >= (int)m_curves.size())
	{
		return m_curves.back().m_endPos;
	}

	float localT = totalT - (float)curveIndex;
	return m_curves[curveIndex].EvaluateAtParametric(localT);
}

float CubicHermiteSpline2D::GetApproximateLength(int numSubdivisionsPerCurve) const
{
	float length = 0.f;

	for (CubicHermiteCurve2D const& curve : m_curves)
	{
		length += curve.GetApproximateLength(numSubdivisionsPerCurve);
	}

	return length;
}

Vec2 CubicHermiteSpline2D::EvaluateAtApproximateDistance(float distanceAlongSpline, int numSubdivisionsPerCurve) const
{
	if (m_curves.empty())
	{
		return Vec2::ZERO;
	}

	if (distanceAlongSpline <= 0.f)
	{
		return m_curves.front().m_startPos;
	}

	float remainingDistance = distanceAlongSpline;

	for (CubicHermiteCurve2D const& curve : m_curves)
	{
		float curveLength = curve.GetApproximateLength(numSubdivisionsPerCurve);

		if (remainingDistance <= curveLength)
		{
			return curve.EvaluateAtApproximateDistance(remainingDistance, numSubdivisionsPerCurve);
		}

		remainingDistance -= curveLength;
	}

	return m_curves.back().m_endPos;
}