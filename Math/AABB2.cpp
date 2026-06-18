#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

const AABB2 AABB2::ZERO_TO_ONE = AABB2(0.f, 0.f, 1.f, 1.f);

AABB2::AABB2(AABB2 const& copyFrom)
:m_mins( copyFrom.m_mins )
,m_maxs( copyFrom.m_maxs )
{
}

AABB2::AABB2(Vec2 const& mins, Vec2 const& maxs)
:m_mins( mins )
,m_maxs( maxs )
{
}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
:m_mins( Vec2( minX, minY ) )
,m_maxs( Vec2( maxX, maxY ) )
{
}

bool AABB2::IsPointInside(Vec2 const& point) const
{
	if (point.x <= m_mins.x || point.x >= m_maxs.x)
	{
		return false;
	}
	else if (point.y <= m_mins.y || point.y >= m_maxs.y)
	{
		return false;
	}
	else
	{
		return true;
	}
}

Vec2 const AABB2::GetCenter() const
{
	float centerX = m_mins.x + ( (m_maxs.x - m_mins.x ) / 2 ) ;
	float centerY = m_mins.y + ( ( m_maxs.y - m_mins.y ) / 2);
	return Vec2(centerX, centerY);
}

Vec2 const AABB2::GetDimensions() const
{
	return Vec2(m_maxs.x - m_mins.x, m_maxs.y - m_mins.y);
}

Vec2 const AABB2::GetNearestPoint(Vec2 const& referencePosition) const
{
	Vec2 nearestPosition = referencePosition;

	if (referencePosition.x < m_mins.x)
	{
		nearestPosition.x = nearestPosition.x + (m_mins.x - referencePosition.x);
	}
	if (referencePosition.x > m_maxs.x)
	{
		nearestPosition.x = nearestPosition.x + (m_maxs.x - referencePosition.x);
	}
	if (referencePosition.y < m_mins.y)
	{
		nearestPosition.y = nearestPosition.y + (m_mins.y - referencePosition.y);
	}
	if (referencePosition.y > m_maxs.y)
	{
		nearestPosition.y = nearestPosition.y + (m_maxs.y - referencePosition.y);
	}
	return nearestPosition;

}

Vec2 const AABB2::GetPointAtUV(Vec2 const& uv) const
{
	return Vec2( Interpolate(m_mins.x, m_maxs.x , uv.x), Interpolate(m_mins.y, m_maxs.y , uv.y) );
}

Vec2 const AABB2::GetUVForPoint(Vec2 const& point) const
{
	return Vec2( GetFractionWithinRange( point.x, m_mins.x, m_maxs.x ), GetFractionWithinRange( point.y, m_mins.y, m_maxs.y ) );
}

void AABB2::Translate(Vec2 const& translation)
{
	m_maxs += translation;
	m_mins += translation;
}

void AABB2::SetCenter(Vec2 const& newCenter)
{
	Vec2 diffNewCenterToOldCenter = newCenter - GetCenter();
	m_maxs += diffNewCenterToOldCenter;
	m_mins += diffNewCenterToOldCenter;
}

void AABB2::SetDimensions(Vec2 const& newDimensions)
{
	Vec2 center = GetCenter();
	Vec2 halfDimensions = Vec2(newDimensions.x * 0.5f, newDimensions.y * 0.5f);
	m_mins = Vec2(center.x - halfDimensions.x, center.y - halfDimensions.y);
	m_maxs = Vec2(center.x + halfDimensions.x, center.y + halfDimensions.y);
}

void AABB2::SetCenterAndDimensions(Vec2 const& newCenter, Vec2 const& newDimensions)
{
	Vec2 halfDimensions = Vec2(newDimensions.x * 0.5f, newDimensions.y * 0.5f);
	m_mins = Vec2(newCenter.x - halfDimensions.x, newCenter.y - halfDimensions.y);
	m_maxs = Vec2(newCenter.x + halfDimensions.x, newCenter.y + halfDimensions.y);
}

void AABB2::StretchToIncludePoint(Vec2 const& point)
{
	if (point.x < m_mins.x)
	{
		m_mins.x = point.x;
	}
	if (point.x > m_maxs.x)
	{
		m_maxs.x = point.x;
	}
	if (point.y < m_mins.y)
	{
		m_mins.y = point.y;
	}
	if (point.y > m_maxs.y)
	{
		m_maxs.y = point.y;
	}
}

