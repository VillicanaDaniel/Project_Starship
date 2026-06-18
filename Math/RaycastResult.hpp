#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/OBB3D.hpp"
#include <vector>

struct RaycastResult2D
{
	// Basic raycast result information (required)
	bool	m_didImpact = false;
	float	m_impactDist = 0.f;
	Vec2	m_impactPos;
	Vec2	m_impactNormal;

	// Original raycast information (optional)
	Vec2	m_rayStartPos;
	Vec2	m_rayFwdNormal;
	float	m_rayMaxLength = 1.f;

	RaycastResult2D& operator=(RaycastResult2D const& copy) = default;
};

struct RaycastResult3D
{
	Vec3 m_rayStartPosition;
	Vec3 m_rayDirection;
	float m_rayLength = 1.f;
	bool m_didImpact = false;
	float m_impactDistance = 0.f;
	Vec3 m_impactPosition;
	Vec3 m_impactNormal;
};

struct RaySegment2D
{
	Vec2 m_start;
	Vec2 m_end;
	bool m_didImpact = false;
};

struct DiscRaycastResult : public RaycastResult2D
{
	int m_impactDiscIndex = -1;

	void operator=(RaycastResult2D const& raycastResult)
	{
		RaycastResult2D::operator=(raycastResult);
	}
};

struct LineSegmentRaycastResult : public RaycastResult2D
{
	int m_impactLineSegmentIndex = -1;

	void operator=(RaycastResult2D const& raycastResult)
	{
		RaycastResult2D::operator=(raycastResult);
	}
};

struct AABB2RaycastResult : public RaycastResult2D
{
	int m_impactAABBIndex = -1;

	void operator=(RaycastResult2D const& raycastResult)
	{
		RaycastResult2D::operator=(raycastResult);
	}
};

RaycastResult2D RaycastVSDiscs2D(Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 discCenter, float discRadius);
RaycastResult2D RaycastVSLineSegment2D(Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 lineStart, Vec2 lineEnd);
RaycastResult2D RaycastVSAABB2D(Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 mins, Vec2 maxs);
RaycastResult3D RaycastVsCylinderZ3D(Vec3 const& start, Vec3 const& direction, float distance, Vec2 const& center, float minZ, float maxZ, float radius);
RaycastResult3D RaycastVsAABB3D(Vec3 const& rayStart, Vec3 const& rayDirection, float rayLength, AABB3 const& box);
RaycastResult3D RaycastVsSphere3D(Vec3 const& rayStart, Vec3 const& rayDirection, float rayLength, Vec3 const& sphereCenter, float sphereRadius);
RaycastResult3D RaycastVsPlane3D(Vec3 const& start, Vec3 const& fwdNormal, float maxDist, Plane3 const& plane);
RaycastResult3D RaycastVsOBB3D(Vec3 const& rayStart, Vec3 const& rayDirection, float rayLength, OBB3 const& obb);