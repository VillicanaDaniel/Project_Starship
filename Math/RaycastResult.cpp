#include "Engine/Math/RaycastResult.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane3.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Raycast Stuff
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVSDiscs2D(Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 discCenter, float discRadius)
{
	RaycastResult2D result;

	Vec2 const& iBasis = rayFwdNormal;
	Vec2 jBasis = rayFwdNormal.GetRotatedBy90Degrees();
	Vec2 SC = discCenter - rayStart;

	// Check if disc is too far to left or right
	float SCj = DotProduct2D(SC, jBasis);
	if (SCj >= discRadius || SCj <= -discRadius)
	{
		return result; // miss
	}

	// Check if disc is too late (after end) or too early (before start)
	float SCi = DotProduct2D(SC, iBasis);
	if (SCi >= rayLength + discRadius || SCi <= -discRadius)
	{
		return result;
	}

	// Check if raycast started inside disc
	if (IsPointInsideDisc2D(rayStart, discCenter, discRadius))
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = rayStart;
		result.m_impactNormal = -rayFwdNormal;

		return result;
	}

	float radiusOffsetDistance = sqrtf((discRadius * discRadius) - (SCj * SCj));
	float impactDist = SCi - radiusOffsetDistance;

	// Check if impact dist is too late or too early 
	if (impactDist >= rayLength || impactDist <= 0.f)
	{
		return result;
	}

	// Raycast did impact
	result.m_didImpact = true;
	result.m_impactDist = impactDist;
	result.m_impactPos = rayStart + (rayFwdNormal * result.m_impactDist);
	result.m_impactNormal = (result.m_impactPos - discCenter).GetNormalized();

	return result;
}

RaycastResult2D RaycastVSLineSegment2D(Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 lineStart, Vec2 lineEnd)
{
	RaycastResult2D result;

	Vec2 rayVec = rayFwdNormal * rayLength;
	Vec2 lineVec = lineEnd - lineStart;
	Vec2 startToLine = lineStart - rayStart;

	float rayLineCross = CrossProduct2D(rayVec, lineVec);
	float const offset = 0.000001f;

	// Parallel (or nearly parallel)
	if (fabsf(rayLineCross) < offset)
	{
		return result;
	}

	float t = CrossProduct2D(startToLine, lineVec) / rayLineCross;
	float u = CrossProduct2D(startToLine, rayVec) / rayLineCross;

	if (t < 0.f || t > 1.f)
	{
		return result;
	}

	if (u < 0.f || u > 1.f)
	{
		return result;
	}

	result.m_didImpact = true;
	result.m_impactPos = rayStart + rayVec * t;
	result.m_impactDist = rayLength * t;

	Vec2 lineDir = lineVec.GetNormalized();
	Vec2 normal = lineDir.GetRotatedBy90Degrees();

	if (DotProduct2D(rayFwdNormal, normal) > 0.f)
	{
		normal = -normal;
	}

	result.m_impactNormal = normal;
	return result;
}

RaycastResult2D RaycastVSAABB2D(Vec2 rayStart, Vec2 rayFwdNormal, float rayLength, Vec2 mins, Vec2 maxs)
{
	RaycastResult2D result;

	Vec2 rayEnd = rayStart + (rayFwdNormal * rayLength);

	if (rayStart.x < mins.x && rayEnd.x < mins.x) return result;
	if (rayStart.x > maxs.x && rayEnd.x > maxs.x) return result;
	if (rayStart.y < mins.y && rayEnd.y < mins.y) return result;
	if (rayStart.y > maxs.y && rayEnd.y > maxs.y) return result;

	float rayXDirection = rayEnd.x - rayStart.x;
	float rayYDirection = rayEnd.y - rayStart.y;

	float txMin = -999999.f;
	float txMax = 999999.f;
	float tyMin = -999999.f;
	float tyMax = 999999.f;

	Vec2 xEntryNormal(0.f, 0.f);
	Vec2 yEntryNormal(0.f, 0.f);

	// X slab
	if (rayXDirection == 0.f)
	{
		if (rayStart.x < mins.x || rayStart.x > maxs.x)
			return result;
	}
	else
	{
		float tXMins = (mins.x - rayStart.x) / rayXDirection;
		float tXMaxs = (maxs.x - rayStart.x) / rayXDirection;

		if (tXMins < tXMaxs)
		{
			txMin = tXMins;
			txMax = tXMaxs;
			xEntryNormal = Vec2(-1.f, 0.f);
		}
		else
		{
			txMin = tXMaxs;
			txMax = tXMins;
			xEntryNormal = Vec2(1.f, 0.f);
		}
	}

	// Y slab
	if (rayYDirection == 0.f)
	{
		if (rayStart.y < mins.y || rayStart.y > maxs.y)
			return result;
	}
	else
	{
		float tYMins = (mins.y - rayStart.y) / rayYDirection;
		float tYMaxs = (maxs.y - rayStart.y) / rayYDirection;

		if (tYMins < tYMaxs)
		{
			tyMin = tYMins;
			tyMax = tYMaxs;
			yEntryNormal = Vec2(0.f, -1.f);
		}
		else
		{
			tyMin = tYMaxs;
			tyMax = tYMins;
			yEntryNormal = Vec2(0.f, 1.f);
		}
	}

	float tEnter = std::max(txMin, tyMin);
	float tExit = std::min(txMax, tyMax);

	if (tEnter > tExit)
		return result;

	if (tExit < 0.f)
		return result;

	if (tEnter < 0.f)
		tEnter = 0.f;

	if (tEnter > 1.f)
		return result;

	float t = tEnter;

	float xImpactPos = RangeMap(t, 0.f, 1.f, rayStart.x, rayEnd.x);
	float yImpactPos = RangeMap(t, 0.f, 1.f, rayStart.y, rayEnd.y);

	result.m_didImpact = true;
	result.m_impactPos = Vec2(xImpactPos, yImpactPos);
	result.m_impactDist = GetDistance2D(rayStart, result.m_impactPos);

	if (txMin > tyMin)
		result.m_impactNormal = xEntryNormal;
	else
		result.m_impactNormal = yEntryNormal;

	return result;
}

RaycastResult3D RaycastVsCylinderZ3D(
	Vec3 const& rayStart,
	Vec3 const& rayDirection,
	float rayLength,
	Vec2 const& cylinderCenterXY,
	float cylinderMinZ,
	float cylinderMaxZ,
	float cylinderRadius
)
{
	RaycastResult3D result;
	result.m_rayStartPosition = rayStart;
	result.m_rayDirection = rayDirection;
	result.m_rayLength = rayLength;

	Vec3 fwd = rayDirection.GetNormalized();

	// Starting inside check
	Vec2 startXY(rayStart.x, rayStart.y);
	Vec2 toStart = startXY - cylinderCenterXY;
	float distSqXY = toStart.GetLengthSquared();

	bool insideXY = distSqXY < (cylinderRadius * cylinderRadius);
	bool insideZ = (rayStart.z >= cylinderMinZ && rayStart.z <= cylinderMaxZ);

	if (insideXY && insideZ)
	{
		result.m_didImpact = true;
		result.m_impactDistance = 0.f;
		result.m_impactPosition = rayStart;

		float distXY = sqrtf(distSqXY);
		float distToSide = cylinderRadius - distXY;
		float distToBottom = rayStart.z - cylinderMinZ;
		float distToTop = cylinderMaxZ - rayStart.z;

		if (distToBottom <= distToSide && distToBottom <= distToTop)
		{
			result.m_impactNormal = Vec3(0.f, 0.f, -1.f);
		}
		else if (distToTop <= distToSide && distToTop <= distToBottom)
		{
			result.m_impactNormal = Vec3(0.f, 0.f, 1.f);
		}
		else
		{
			Vec2 outward = toStart.GetNormalized();
			result.m_impactNormal = Vec3(outward.x, outward.y, 0.f);
		}

		return result;
	}

	float bestT = rayLength + 1.f;
	Vec3 bestNormal = Vec3::ZERO;
	Vec3 bestPos = Vec3::ZERO;
	bool foundHit = false;

	// Side wall intersection
	Vec2 startToCenter(rayStart.x - cylinderCenterXY.x, rayStart.y - cylinderCenterXY.y);
	Vec2 dirXY(fwd.x, fwd.y);

	float a = DotProduct2D(dirXY, dirXY);
	float b = 2.f * DotProduct2D(startToCenter, dirXY);
	float c = DotProduct2D(startToCenter, startToCenter) - (cylinderRadius * cylinderRadius);

	if (a > 0.f)
	{
		float discriminant = b * b - 4.f * a * c;
		if (discriminant >= 0.f)
		{
			float sqrtDisc = sqrtf(discriminant);

			float t1 = (-b - sqrtDisc) / (2.f * a);
			float t2 = (-b + sqrtDisc) / (2.f * a);

			float candidates[2] = { t1, t2 };
			for (int i = 0; i < 2; ++i)
			{
				float t = candidates[i];
				if (t < 0.f || t > rayLength || t >= bestT)
				{
					continue;
				}

				Vec3 hitPos = rayStart + fwd * t;
				if (hitPos.z < cylinderMinZ || hitPos.z > cylinderMaxZ)
				{
					continue;
				}

				Vec2 outwardXY(hitPos.x - cylinderCenterXY.x, hitPos.y - cylinderCenterXY.y);
				outwardXY = outwardXY.GetNormalized();

				bestT = t;
				bestPos = hitPos;
				bestNormal = Vec3(outwardXY.x, outwardXY.y, 0.f);
				foundHit = true;
			}
		}
	}

	// Bottom cap intersection
	if (fwd.z != 0.f)
	{
		float tBottom = (cylinderMinZ - rayStart.z) / fwd.z;
		if (tBottom >= 0.f && tBottom <= rayLength && tBottom < bestT)
		{
			Vec3 hitPos = rayStart + fwd * tBottom;
			Vec2 hitXY(hitPos.x, hitPos.y);
			if ((hitXY - cylinderCenterXY).GetLengthSquared() <= (cylinderRadius * cylinderRadius))
			{
				bestT = tBottom;
				bestPos = hitPos;
				bestNormal = Vec3(0.f, 0.f, -1.f);
				foundHit = true;
			}
		}

		// Top cap intersection
		float tTop = (cylinderMaxZ - rayStart.z) / fwd.z;
		if (tTop >= 0.f && tTop <= rayLength && tTop < bestT)
		{
			Vec3 hitPos = rayStart + fwd * tTop;
			Vec2 hitXY(hitPos.x, hitPos.y);
			if ((hitXY - cylinderCenterXY).GetLengthSquared() <= (cylinderRadius * cylinderRadius))
			{
				bestT = tTop;
				bestPos = hitPos;
				bestNormal = Vec3(0.f, 0.f, 1.f);
				foundHit = true;
			}
		}
	}

	if (foundHit)
	{
		result.m_didImpact = true;
		result.m_impactDistance = bestT;
		result.m_impactPosition = bestPos;
		result.m_impactNormal = bestNormal;
	}

	return result;
}

RaycastResult3D RaycastVsAABB3D(Vec3 const& rayStart, Vec3 const& rayDirection, float rayLength, AABB3 const& box)
{
	RaycastResult3D result;
	result.m_rayStartPosition = rayStart;
	result.m_rayDirection = rayDirection;
	result.m_rayLength = rayLength;

	Vec3 fwd = rayDirection.GetNormalized();

	float tEnter = 0.f;
	float tExit = rayLength;
	Vec3 entryNormal = Vec3::ZERO;

	// X slab
	if (fwd.x == 0.f)
	{
		if (rayStart.x < box.m_mins.x || rayStart.x > box.m_maxs.x)
		{
			return result;
		}
	}
	else
	{
		float txMin = (box.m_mins.x - rayStart.x) / fwd.x;
		float txMax = (box.m_maxs.x - rayStart.x) / fwd.x;
		Vec3 xNormal = Vec3(-1.f, 0.f, 0.f);

		if (txMin > txMax)
		{
			float temp = txMin;
			txMin = txMax;
			txMax = temp;
			xNormal = Vec3(1.f, 0.f, 0.f);
		}

		if (txMin > tEnter)
		{
			tEnter = txMin;
			entryNormal = xNormal;
		}

		if (txMax < tExit)
		{
			tExit = txMax;
		}
	}

	// Y slab
	if (fwd.y == 0.f)
	{
		if (rayStart.y < box.m_mins.y || rayStart.y > box.m_maxs.y)
		{
			return result;
		}
	}
	else
	{
		float tyMin = (box.m_mins.y - rayStart.y) / fwd.y;
		float tyMax = (box.m_maxs.y - rayStart.y) / fwd.y;
		Vec3 yNormal = Vec3(0.f, -1.f, 0.f);

		if (tyMin > tyMax)
		{
			float temp = tyMin;
			tyMin = tyMax;
			tyMax = temp;
			yNormal = Vec3(0.f, 1.f, 0.f);
		}

		if (tyMin > tEnter)
		{
			tEnter = tyMin;
			entryNormal = yNormal;
		}

		if (tyMax < tExit)
		{
			tExit = tyMax;
		}
	}

	// Z slab
	if (fwd.z == 0.f)
	{
		if (rayStart.z < box.m_mins.z || rayStart.z > box.m_maxs.z)
		{
			return result;
		}
	}
	else
	{
		float tzMin = (box.m_mins.z - rayStart.z) / fwd.z;
		float tzMax = (box.m_maxs.z - rayStart.z) / fwd.z;
		Vec3 zNormal = Vec3(0.f, 0.f, -1.f);

		if (tzMin > tzMax)
		{
			float temp = tzMin;
			tzMin = tzMax;
			tzMax = temp;
			zNormal = Vec3(0.f, 0.f, 1.f);
		}

		if (tzMin > tEnter)
		{
			tEnter = tzMin;
			entryNormal = zNormal;
		}

		if (tzMax < tExit)
		{
			tExit = tzMax;
		}
	}

	if (tEnter > tExit)
	{
		return result;
	}

	if (tExit < 0.f)
	{
		return result;
	}

	if (tEnter < 0.f)
	{
		tEnter = 0.f;
		entryNormal = -fwd;
	}

	if (tEnter > rayLength)
	{
		return result;
	}

	result.m_didImpact = true;
	result.m_impactDistance = tEnter;
	result.m_impactPosition = rayStart + fwd * tEnter;
	result.m_impactNormal = entryNormal;

	return result;
}

RaycastResult3D RaycastVsSphere3D(Vec3 const& rayStart, Vec3 const& rayDirection, float rayLength, Vec3 const& sphereCenter, float sphereRadius)
{
	RaycastResult3D result;
	result.m_rayStartPosition = rayStart;
	result.m_rayDirection = rayDirection;
	result.m_rayLength = rayLength;

	Vec3 fwd = rayDirection.GetNormalized();

	Vec3 startToCenter = sphereCenter - rayStart;
	float projectedDist = DotProduct3D(startToCenter, fwd);

	Vec3 nearestPointOnRay = rayStart + fwd * projectedDist;
	float distToCenterSq = (sphereCenter - nearestPointOnRay).GetLengthSquared();
	float radiusSq = sphereRadius * sphereRadius;

	if (distToCenterSq > radiusSq)
	{
		return result;
	}

	float halfChord = sqrtf(radiusSq - distToCenterSq);

	float impactDistance = projectedDist - halfChord;

	if (impactDistance < 0.f)
	{
		impactDistance = projectedDist + halfChord;
	}

	if (impactDistance < 0.f || impactDistance > rayLength)
	{
		return result;
	}

	result.m_didImpact = true;
	result.m_impactDistance = impactDistance;
	result.m_impactPosition = rayStart + fwd * impactDistance;
	result.m_impactNormal = (result.m_impactPosition - sphereCenter).GetNormalized();

	return result;
}

RaycastResult3D RaycastVsPlane3D(Vec3 const& start, Vec3 const& fwdNormal, float maxDist, Plane3 const& plane)
{
	RaycastResult3D result;
	result.m_rayStartPosition = start;
	result.m_rayDirection = fwdNormal;
	result.m_rayLength = maxDist;

	float startAltitude =
		DotProduct3D(start, plane.m_normal) - plane.m_distFromOrigin;

	float forwardDot =
		DotProduct3D(fwdNormal, plane.m_normal);

	if (fabsf(forwardDot) < 0.000001f)
	{
		return result;
	}

	float impactDist = -startAltitude / forwardDot;

	if (impactDist < 0.f || impactDist > maxDist)
	{
		return result;
	}

	result.m_didImpact = true;
	result.m_impactDistance = impactDist;
	result.m_impactPosition = start + fwdNormal * impactDist;

	result.m_impactNormal = plane.m_normal;
	if (DotProduct3D(result.m_impactNormal, fwdNormal) > 0.f)
	{
		result.m_impactNormal = -result.m_impactNormal;
	}

	return result;
}

RaycastResult3D RaycastVsOBB3D(
	Vec3 const& rayStart,
	Vec3 const& rayDirection,
	float rayLength,
	OBB3 const& obb)
{
	Vec3 displacement = rayStart - obb.m_center;

	Vec3 localStart(
		DotProduct3D(displacement, obb.m_iBasis),
		DotProduct3D(displacement, obb.m_jBasis),
		DotProduct3D(displacement, obb.m_kBasis)
	);

	Vec3 localDirection(
		DotProduct3D(rayDirection, obb.m_iBasis),
		DotProduct3D(rayDirection, obb.m_jBasis),
		DotProduct3D(rayDirection, obb.m_kBasis)
	);

	AABB3 localBounds(
		-obb.m_halfDimensions,
		obb.m_halfDimensions
	);

	RaycastResult3D localResult =
		RaycastVsAABB3D(localStart, localDirection, rayLength, localBounds);

	RaycastResult3D result = localResult;
	result.m_rayStartPosition = rayStart;
	result.m_rayDirection = rayDirection;
	result.m_rayLength = rayLength;

	if (!localResult.m_didImpact)
	{
		return result;
	}

	Vec3 localImpact = localResult.m_impactPosition;
	Vec3 localNormal = localResult.m_impactNormal;

	result.m_impactPosition =
		obb.m_center +
		obb.m_iBasis * localImpact.x +
		obb.m_jBasis * localImpact.y +
		obb.m_kBasis * localImpact.z;

	result.m_impactNormal =
		obb.m_iBasis * localNormal.x +
		obb.m_jBasis * localNormal.y +
		obb.m_kBasis * localNormal.z;

	result.m_impactNormal = result.m_impactNormal.GetNormalized();

	return result;
}