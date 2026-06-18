#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/OBB3D.hpp"
#include <cmath>
#include <iostream>

float ConvertDegreesToRadians(float degrees)
{
	return (degrees * (3.141592653589f / 180.f));
}

float ConvertRadiansToDegrees(float radians)
{
	return (radians * (180.f / 3.141592653589f));
}

float CosDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return cosf(radians); 
}
float SinDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return sinf(radians);
}
float Atan2Degrees(float y, float x)
{
	float radians = atan2f(y, x); 
	float degrees = ConvertRadiansToDegrees(radians); 
	return degrees;
}


// Basic 2D & 3D utilities
float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB)
{
	return sqrt(GetDistanceSquared2D( positionA, positionB ));
}
float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB)
{
	float distance_x = positionB.x - positionA.x;
	float distance_y = positionB.y - positionA.y;
	return (distance_x * distance_x + distance_y * distance_y);
}
float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return sqrt(GetDistanceSquared3D( positionA, positionB ));
}
float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	float distance_x = positionB.x - positionA.x;
	float distance_y = positionB.y - positionA.y;
	float distance_z = positionB.z - positionA.z;
	return (distance_x * distance_x + distance_y * distance_y + distance_z * distance_z);
}
float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return sqrt(GetDistanceXYSquared3D( positionA, positionB ));
}
float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	float distance_x = positionB.x - positionA.x;
	float distance_y = positionB.y - positionA.y;
	return (distance_x * distance_x + distance_y * distance_y);
}

// Geometric query utilities
bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB)
{
	float dx = centerB.x - centerA.x;
	float dy = centerB.y - centerA.y;

	float distanceSquared = dx * dx + dy * dy;
	float radiusSum = radiusA + radiusB;
	float radiusSumSquared = radiusSum * radiusSum;

	return distanceSquared < radiusSumSquared;
}
bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{
	float dx = centerB.x - centerA.x;
	float dy = centerB.y - centerA.y;
	float dz = centerB.z - centerA.z;

	float distanceSquared = dx * dx + dy * dy + dz * dz;
	float radiusSum = radiusA + radiusB;
	float radiusSumSquared = radiusSum * radiusSum;

	return distanceSquared < radiusSumSquared;
}

// Transform utilities
void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation)
{
	float distXY = posToTransform.GetLength();
	distXY *= uniformScale;

	// Step 2: Rotate around Z axis 
	float thetaDegrees = Atan2Degrees(posToTransform.y, posToTransform.x);

	thetaDegrees += rotationDegrees;


	posToTransform.x = translation.x + (distXY * CosDegrees(thetaDegrees));

	posToTransform.y = translation.y + (distXY * SinDegrees(thetaDegrees));
}

void TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	posToTransform = (iBasis * posToTransform.x) + (jBasis * posToTransform.y) + translation;
}

void TransformPositionXY3D(Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation)
{

	float rotationRadians = ConvertDegreesToRadians(zRotationDegrees);
	// Step 1: Scale X and Y
	float distXY = posToTransform.GetLengthXY();
	distXY *= xyScale;

	// Step 2: Rotate around Z axis 
	float thetaRadians = atan2f(posToTransform.y , posToTransform.x);

	thetaRadians += rotationRadians;


	posToTransform.x = xyTranslation.x + (distXY * cosf(thetaRadians)) ;

	posToTransform.y = xyTranslation.y + (distXY * sinf(thetaRadians)) ;
}

void TransformPositionXY3D(Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	float newX = (iBasis.x * posToTransform.x) + (jBasis.x * posToTransform.y) + translation.x;
	float newY = (iBasis.y * posToTransform.x) + (jBasis.y * posToTransform.y) + translation.y;
	posToTransform.x = newX;
	posToTransform.y = newY;
}

float Interpolate(float start, float end, float tWeightOfEnd)
{
	return (start + (tWeightOfEnd * (end - start)));
}

Vec2 InterpolateVec2(Vec2 const& start, Vec2 const& end, float t)
{
	return start + (end - start) * t;
}

float GetFractionWithinRange(float value, float start, float end)
{
	if (end - start == 0.0)
	{
		return 0.5f;
	}
	
	return ((value - start) / (end - start));
}

float GetClamped(float value, float minValue, float maxValue)
{
	if (value > maxValue)
	{
		return maxValue;
	}
	
	if (value < minValue)
	{
		return minValue;
	}

	return value;
}

float GetClampedZeroToOne(float value)
{
	if (value > 1.0f)
	{
		return 1.0f;
	}
	else if (value < 0.0f)
	{
		return 0.0f;
	}
	else
		return value;
}

float DotProduct2D(Vec2 const& a, Vec2 const& b)
{
 return (a.x * b.x) + (a.y * b.y);
}

float GetShortestAngularDispDegrees(float startDegrees, float endDegrees)
{
	float start = fmodf(startDegrees, 360.f);
	if (start < 0) 
		start += 360.f;

	float end = fmodf(endDegrees, 360.f);
	if (end < 0) 
		end += 360.f;

	float delta = end - start;

	if (delta > 180.f)
		delta -= 360.f;
	else if (delta < -180.f)
		delta += 360.f;

	return delta;
}

float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees)
{
	float delta = GetShortestAngularDispDegrees(currentDegrees, goalDegrees);

	// Clamp delta to maxDeltaDegrees
	if (fabsf(delta) <= maxDeltaDegrees)
		return fmodf(goalDegrees + 360.f, 360.f);  // Can move directly to goal

	// Move a step toward the goal
	float step = (delta > 0.f ? 1.f : -1.f) * maxDeltaDegrees;
	float newAngle = currentDegrees + step;

	// Normalize to [0, 360)
	newAngle = fmodf(newAngle, 360.f);
	if (newAngle < 0.f)
		newAngle += 360.f;

	return newAngle;
}

int GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB)
{
	int dx = abs(pointB.x - pointA.x);
	int dy = abs(pointB.y - pointA.y);
	return dx + dy;
}

float GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	Vec2 normalizedVectorToProjectOnto = vectorToProjectOnto.GetNormalized();

	return DotProduct2D(normalizedVectorToProjectOnto, vectorToProject);
}

Vec2 GetProjectedVector2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	Vec2 normalizedVectorToProjectOnto = vectorToProjectOnto.GetNormalized();

	float ProjectedLength = GetProjectedLength2D(vectorToProject, vectorToProjectOnto);

	return ProjectedLength * normalizedVectorToProjectOnto;
}

bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius)
{
	float distSquaredPointToCenter = GetDistanceSquared2D( point, discCenter );
	return distSquaredPointToCenter < (discRadius * discRadius);
}

bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius)
{
	if ( !IsPointInsideDisc2D(point, sectorTip, sectorRadius ) )
	{
		return false;
	}

	Vec2 tipToPoint = point - sectorTip;
	float anglDispDeg = GetShortestAngularDispDegrees( tipToPoint.GetOrientationDegrees(), sectorFwdDegrees );
	if (fabs(anglDispDeg) >= sectorApertureDegrees * 0.5f)
	{
		return false;
	}

	return true;
}

bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorFwdNormal, float sectorApertureDegrees, float sectorRadius)
{
	if ( !IsPointInsideDisc2D( point, sectorTip, sectorRadius ) )
	{
		return false;
	}

	float minimumDotProduct = CosDegrees( sectorApertureDegrees * 0.5f );
	Vec2 tipToPoint = point - sectorTip;
	float actualDotProduct = DotProduct2D( sectorFwdNormal, tipToPoint.GetNormalized() );
	return actualDotProduct > minimumDotProduct;
}

Vec2 GetNearestPointOnDisc2D(Vec2 const& referencePos, Vec2 const& discCenter, float discRadius)
{
	Vec2 dispToRefPos = referencePos - discCenter;
	dispToRefPos.ClampLength( discRadius );
	return discCenter + dispToRefPos;
}

bool PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint)
{
	Vec2 displacement = mobileDiscCenter - fixedPoint;
	float dist = displacement.GetLength();

	if (dist >= discRadius)
	{
		return false; //Outside of Point
	}

	float overlap = discRadius - dist;
	Vec2 pushDir = displacement.GetNormalized();
	mobileDiscCenter += pushDir * overlap;
	return true;
}

bool PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius)
{
	Vec2 displacement = mobileDiscCenter - fixedDiscCenter;
	float dist = displacement.GetLength();
	float combinedRadius = discRadius + fixedDiscRadius;

	if (dist >= combinedRadius)
	{
		return false; //Outside of Fixed Disc
	}

	float overlap = combinedRadius - dist;
	Vec2 pushDir = displacement.GetNormalized();
	mobileDiscCenter += pushDir * overlap;
	return true;
}

bool PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius)
{
	Vec2 displacement = bCenter - aCenter;
	float dist = displacement.GetLength();
	float combinedRadius = aRadius + bRadius;

	if (dist >= combinedRadius)
	{
		return false; //Discs Outside of Each Other
	}

	float overlap = combinedRadius - dist;
	Vec2 pushDir = displacement.GetNormalized();

	Vec2 move = pushDir * (overlap * 0.5f); //Push Each Disc Equal Distance
	aCenter -= move;
	bCenter += move;
	return true;
}

bool PushDiscOutOfFixedAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox)
{
	Vec2 nearestPoint = GetNearestPointOnAABB2D(mobileDiscCenter, fixedBox);
	Vec2 displacement = mobileDiscCenter - nearestPoint;
	float dist = displacement.GetLength();

	if (dist >= discRadius)
	{
		return false;
	}

	if (dist > 0.f)
	{
		Vec2 pushDir = displacement.GetNormalized();
		float overlap = discRadius - dist;
		mobileDiscCenter += pushDir * overlap;
		return true;
	}

	// Special case: center is inside the box or exactly on nearest point
	float pushLeft = fabsf(mobileDiscCenter.x - fixedBox.m_mins.x);
	float pushRight = fabsf(fixedBox.m_maxs.x - mobileDiscCenter.x);
	float pushDown = fabsf(mobileDiscCenter.y - fixedBox.m_mins.y);
	float pushUp = fabsf(fixedBox.m_maxs.y - mobileDiscCenter.y);

	float minPush = pushLeft;
	Vec2 pushDir(-1.f, 0.f);

	if (pushRight < minPush)
	{
		minPush = pushRight;
		pushDir = Vec2(1.f, 0.f);
	}
	if (pushDown < minPush)
	{
		minPush = pushDown;
		pushDir = Vec2(0.f, -1.f);
	}
	if (pushUp < minPush)
	{
		minPush = pushUp;
		pushDir = Vec2(0.f, 1.f);
	}

	mobileDiscCenter += pushDir * (discRadius + minPush);
	return true;
}

float GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b)
{
	float cosOfAngle = DotProduct2D( a , b ) / ( a.GetLength() * b.GetLength() );
	cosOfAngle = GetClamped( cosOfAngle, -1.f, 1.f );
	float angleRadians = acosf( cosOfAngle );
	return ConvertRadiansToDegrees( angleRadians );
}

bool IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box)
{
	if ( point.x >= box.m_mins.x && point.x <= box.m_maxs.x &&
		point.y >= box.m_mins.y && point.y <= box.m_maxs.y ) 
	{
		return true;
	}

	return false;
}

bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& box)
{
	Vec2 localPos = box.GetLocalPosForWorldPos(point);
	return fabsf(localPos.x) <= box.m_halfDimensions.x && fabsf(localPos.y) <= box.m_halfDimensions.y;
}

bool IsPointInsideTriangle2D(Vec2 const& point, Vec2 const& a, Vec2 const& b, Vec2 const& c)
{
	Vec2 ca = c - a;
	Vec2 ba = b - a;
	Vec2 pa = point - a;

	float dot00 = DotProduct2D(ca, ca);
	float dot01 = DotProduct2D(ca, ba);
	float dot02 = DotProduct2D(ca, pa);
	float dot11 = DotProduct2D(ba, ba);
	float dot12 = DotProduct2D(ba, pa);

	float denom = dot00 * dot11 - dot01 * dot01;

	float invDenom = 1.f / denom;
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return (u >= 0.f) && (v >= 0.f) && (u + v <= 1.f);
}

bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
	Vec2 nearestOnSegment = GetNearestPointOnLineSegment2D(point, boneStart, boneEnd);
	float distSquared = (point - nearestOnSegment).GetLengthSquared();
	return distSquared <= (radius * radius);
}

Vec2 GetNearestPointOnAABB2D(Vec2 const& point, AABB2 const& box)
{
	float clampedX = GetClamped(point.x, box.m_mins.x, box.m_maxs.x);
	float clampedY = GetClamped(point.y, box.m_mins.y, box.m_maxs.y);
	return Vec2(clampedX, clampedY);
}

Vec2 GetNearestPointOnOBB2D(Vec2 const& point, OBB2 const& box)
{
	Vec2 localPos = box.GetLocalPosForWorldPos(point);
	Vec2 clampedLocal = Vec2(GetClamped(localPos.x, -box.m_halfDimensions.x, box.m_halfDimensions.x),
		GetClamped(localPos.y, -box.m_halfDimensions.y, box.m_halfDimensions.y));
	return box.GetWorldPosForLocalPos(clampedLocal);
}

Vec2 GetNearestPointOnLineSegment2D(Vec2 const& point, Vec2 const& start, Vec2 const& end)
{
	Vec2 line = end - start;
	float t = DotProduct2D(point - start, line) / line.GetLengthSquared();
	t = GetClamped(t, 0.f, 1.f);
	return start + (line * t);
}

Vec2 GetNearestPointOnInfiniteLine2D(Vec2 const& point, Vec2 const& linePoint, Vec2 const& lineDir)
{
	Vec2 dirNorm = lineDir.GetNormalized();      // ensure direction is normalized
	float t = DotProduct2D(point - linePoint, dirNorm);
	return linePoint + dirNorm * t;
}

Vec2 GetNearestPointOnTriangle2D(Vec2 const& point, Vec2 const& a, Vec2 const& b, Vec2 const& c)
{
	//HACK WAY OF DOING IT, WILL CHANGE LATER 
	Vec2 pAB = GetNearestPointOnLineSegment2D(point, a, b);
	Vec2 pBC = GetNearestPointOnLineSegment2D(point, b, c);
	Vec2 pCA = GetNearestPointOnLineSegment2D(point, c, a);

	float dAB = GetDistanceSquared2D(point, pAB);
	float dBC = GetDistanceSquared2D(point, pBC);
	float dCA = GetDistanceSquared2D(point, pCA);

	Vec2 nearest = pAB;
	float minDist = dAB;

	if (dBC < minDist)
	{
		minDist = dBC;
		nearest = pBC;
	}
	if (dCA < minDist)
	{
		minDist = dCA;
		nearest = pCA;
	}

	if (IsPointInsideTriangle2D(point, a, b, c))
		return point;

	return nearest;
}

Vec2 GetNearestPointOnCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
	Vec2 nearestOnSegment = GetNearestPointOnLineSegment2D(point, boneStart, boneEnd);
	Vec2 displacement = point - nearestOnSegment;
	float dist = displacement.GetLength();

	if (dist <= radius)
		return point;

	Vec2 dir = displacement / dist;
	return nearestOnSegment + dir * radius;
}

float RangeMap(float inValue, float inRangeStart, float inRangeEnd, float outRangeStart, float outRangeEnd)
{
	return Interpolate( outRangeStart, outRangeEnd, GetFractionWithinRange(inValue, inRangeStart, inRangeEnd));
}

float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	float inValueClamped = GetClamped(inValue, inStart,inEnd);
	return Interpolate( outStart, outEnd, GetFractionWithinRange(inValueClamped, inStart, inEnd));

}

int RoundDownToInt(float value)
{
	return static_cast<int>(floorf(value));
}

// static float ClampFloat(float v, float minV, float maxV)
// {
// 	if (v < minV) return minV;
// 	if (v > maxV) return maxV;
// 	return v;
// }

float DotProduct3D(Vec3 const& a, Vec3 const& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float DotProduct4D(Vec4 const& a, Vec4 const& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

float CrossProduct2D(Vec2 const& a, Vec2 const& b)
{
	return (a.x * b.y) - (a.y * b.x);
}

Vec3 CrossProduct3D(Vec3 const& a, Vec3 const& b)
{
	return Vec3(
		(a.y * b.z) - (a.z * b.y),
		(a.z * b.x) - (a.x * b.z),
		(a.x * b.y) - (a.y * b.x)
	);
}

float NormalizeByte(unsigned char byteValue)
{
	return (float)byteValue / 255.0f;
}

unsigned char DenormalizeByte(float zeroToOne)
{
	if (zeroToOne <= 0.0f) return 0;
	if (zeroToOne >= 1.0f) return 255;

	int bucket = (int)std::floor(zeroToOne * 256.0f);
	return (unsigned char)bucket;
}

Mat44 GetBillboardTransform(BillboardType billboardType, Mat44 const& targetTransform, Vec3 const& billboardPosition, Vec2 const& billboardScale /*= Vec2(1.f, 1.f)*/)
{
	Vec3 worldUp = Vec3(0.f, 0.f, 1.f);
	Mat44 result;

	if (billboardType == BillboardType::NONE)
	{
		Vec3 iBasis = Vec3(1.f, 0.f, 0.f);
		Vec3 jBasis = Vec3(0.f, 1.f, 0.f) * billboardScale.x;
		Vec3 kBasis = Vec3(0.f, 0.f, 1.f) * billboardScale.y;

		result.SetIJKT3D(iBasis, jBasis, kBasis, billboardPosition);
		return result;
	}

	Vec3 cameraPos = targetTransform.GetTranslation3D();
	Vec3 cameraForward = targetTransform.GetIBasis3D().GetNormalized();

	Vec3 iBasis;
	Vec3 jBasis;
	Vec3 kBasis;

	if (billboardType == BillboardType::FULL_FACING)
	{
		// Face camera position
		iBasis = -(cameraPos - billboardPosition).GetNormalized();

		Vec3 referenceUp = targetTransform.GetKBasis3D().GetNormalized();

		if (fabsf(DotProduct3D(referenceUp, iBasis)) > 0.999f)
		{
			referenceUp = targetTransform.GetJBasis3D().GetNormalized();
		}

		jBasis = CrossProduct3D(referenceUp, iBasis).GetNormalized();
		kBasis = CrossProduct3D(iBasis, jBasis).GetNormalized();
	}
	else if (billboardType == BillboardType::FULL_OPPOSING)
	{
		// Oppose camera orientation
		iBasis = cameraForward;

		Vec3 referenceUp = targetTransform.GetKBasis3D().GetNormalized();

		if (fabsf(DotProduct3D(referenceUp, iBasis)) > 0.999f)
		{
			referenceUp = targetTransform.GetJBasis3D().GetNormalized();
		}

		jBasis = CrossProduct3D(referenceUp, iBasis).GetNormalized();
		kBasis = CrossProduct3D(iBasis, jBasis).GetNormalized();
	}
	else if (billboardType == BillboardType::WORLD_UP_FACING)
	{
		// Face camera position, constrained to world-up plane
		iBasis = -(cameraPos - billboardPosition);
		iBasis.z = 0.f;

		if (iBasis.GetLengthSquared() <= 0.000001f)
		{
			iBasis = Vec3(1.f, 0.f, 0.f);
		}
		else
		{
			iBasis = iBasis.GetNormalized();
		}

		jBasis = CrossProduct3D(worldUp, iBasis).GetNormalized();
		kBasis = CrossProduct3D(iBasis, jBasis).GetNormalized();
	}
	else // WORLD_UP_OPPOSING
	{
		// Oppose camera orientation, constrained to world-up plane
		iBasis = cameraForward;
		iBasis.z = 0.f;

		if (iBasis.GetLengthSquared() <= 0.000001f)
		{
			iBasis = Vec3(1.f, 0.f, 0.f);
		}
		else
		{
			iBasis = iBasis.GetNormalized();
		}

		jBasis = CrossProduct3D(worldUp, iBasis).GetNormalized();
		kBasis = CrossProduct3D(iBasis, jBasis).GetNormalized();
	}

	jBasis *= billboardScale.x;
	kBasis *= billboardScale.y;

	result.SetIJKT3D(iBasis, jBasis, kBasis, billboardPosition);
	return result;
}

bool DoAABB3sOverlap3D(AABB3 const& first, AABB3 const& second)
{
	if (first.m_maxs.x < second.m_mins.x || first.m_mins.x > second.m_maxs.x) 
		return false;
	if (first.m_maxs.y < second.m_mins.y || first.m_mins.y > second.m_maxs.y) 
		return false;
	if (first.m_maxs.z < second.m_mins.z || first.m_mins.z > second.m_maxs.z) 
		return false;

	return true;
}

bool DoSpheresOverlap3D(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{
	float radiusSum = radiusA + radiusB;
	return (centerB - centerA).GetLengthSquared() <= radiusSum * radiusSum;
}

bool DoZCylindersOverlap3D(
	Vec2 const& centerA,
	float radiusA,
	float minZA,
	float maxZA,
	Vec2 const& centerB,
	float radiusB,
	float minZB,
	float maxZB)
{
	if (maxZA < minZB || minZA > maxZB)
	{
		return false;
	}

	float radiusSum = radiusA + radiusB;
	return (centerB - centerA).GetLengthSquared() <= radiusSum * radiusSum;
}

bool DoSphereAndAABB3Overlap3D(Vec3 const& sphereCenter, float sphereRadius, AABB3 const& box)
{
	Vec3 nearestPoint(
		GetClamped(sphereCenter.x, box.m_mins.x, box.m_maxs.x),
		GetClamped(sphereCenter.y, box.m_mins.y, box.m_maxs.y),
		GetClamped(sphereCenter.z, box.m_mins.z, box.m_maxs.z)
	);

	return (nearestPoint - sphereCenter).GetLengthSquared() <= sphereRadius * sphereRadius;
}

bool DoZCylinderAndAABB3Overlap3D(
	Vec2 const& cylinderCenterXY,
	float cylinderRadius,
	float cylinderMinZ,
	float cylinderMaxZ,
	AABB3 const& box)
{
	if (cylinderMaxZ < box.m_mins.z || cylinderMinZ > box.m_maxs.z)
	{
		return false;
	}

	Vec2 nearestXY(
		GetClamped(cylinderCenterXY.x, box.m_mins.x, box.m_maxs.x),
		GetClamped(cylinderCenterXY.y, box.m_mins.y, box.m_maxs.y)
	);

	return (nearestXY - cylinderCenterXY).GetLengthSquared() <= cylinderRadius * cylinderRadius;
}

bool DoZCylinderAndSphereOverlap3D(
	Vec2 const& cylinderCenterXY,
	float cylinderRadius,
	float cylinderMinZ,
	float cylinderMaxZ,
	Vec3 const& sphereCenter,
	float sphereRadius)
{
	Vec2 sphereXY(sphereCenter.x, sphereCenter.y);
	Vec2 dispXY = sphereXY - cylinderCenterXY;

	float distXY = dispXY.GetLength();
	Vec2 nearestXY;

	if (distXY == 0.f)
	{
		nearestXY = cylinderCenterXY;
	}
	else
	{
		float clampedDist = GetClamped(distXY, 0.f, cylinderRadius);
		nearestXY = cylinderCenterXY + (dispXY / distXY) * clampedDist;
	}

	float nearestZ = GetClamped(sphereCenter.z, cylinderMinZ, cylinderMaxZ);

	Vec3 nearestPoint(nearestXY.x, nearestXY.y, nearestZ);
	return (nearestPoint - sphereCenter).GetLengthSquared() <= sphereRadius * sphereRadius;
}

float ComputeCubicBezier1D(float A, float B, float C, float D, float t)
{
	float AB = Interpolate(A, B, t);
	float BC = Interpolate(B, C, t);
	float CD = Interpolate(C, D, t);

	float ABC = Interpolate(AB, BC, t);
	float BCD = Interpolate(BC, CD, t);

	float ABCD = Interpolate(ABC, BCD, t);

	return ABCD;
}

float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t)
{
	float AB = Interpolate(A, B, t);
	float BC = Interpolate(B, C, t);
	float CD = Interpolate(C, D, t);
	float DE = Interpolate(D, E, t);
	float EF = Interpolate(E, F, t);

	float ABC = Interpolate(AB, BC, t);
	float BCD = Interpolate(BC, CD, t);
	float CDE = Interpolate(CD, DE, t);
	float DEF = Interpolate(DE, EF, t);

	float ABCD = Interpolate(ABC, BCD, t);
	float BCDE = Interpolate(BCD, CDE, t);
	float CDEF = Interpolate(CDE, DEF, t);

	float ABCDE = Interpolate(ABCD, BCDE, t);
	float BCDEF = Interpolate(BCDE, CDEF, t);

	float ABCDEF = Interpolate(ABCDE, BCDEF, t);

	return ABCDEF;
}

float SmoothStart2(float t)
{
	return t * t;
}

float SmoothStart3(float t)
{
	return t * t * t;
}

float SmoothStart4(float t)
{
	float t2 = t * t;
	return t2 * t2;
}

float SmoothStart5(float t)
{
	float t2 = t * t;
	return t2 * t2 * t;
}

float SmoothStart6(float t)
{
	float t2 = t * t;
	return t2 * t2 * t2;
}

float SmoothStop2(float t)
{
	float oneMinusT = 1.f - t;
	return 1.f - oneMinusT * oneMinusT;
}

float SmoothStop3(float t)
{
	float oneMinusT = 1.f - t;
	return 1.f - oneMinusT * oneMinusT * oneMinusT;
}

float SmoothStop4(float t)
{
	float oneMinusT = 1.f - t;
	float oneMinusT2 = oneMinusT * oneMinusT;
	return 1.f - oneMinusT2 * oneMinusT2;
}

float SmoothStop5(float t)
{
	float oneMinusT = 1.f - t;
	float oneMinusT2 = oneMinusT * oneMinusT;
	return 1.f - oneMinusT2 * oneMinusT2 * oneMinusT;
}

float SmoothStop6(float t)
{
	float oneMinusT = 1.f - t;
	float oneMinusT2 = oneMinusT * oneMinusT;
	return 1.f - oneMinusT2 * oneMinusT2 * oneMinusT2;
}

float SmoothStep3(float t)
{
	return t * t * (3.f - 2.f * t);
}

float SmoothStep5(float t)
{
	return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
}

float Hesitate3(float t)
{
	return ComputeCubicBezier1D(0.f, 1.f, 0.f, 1.f, t);
}

float Hesitate5(float t)
{
	return ComputeQuinticBezier1D(0.f, 1.f, 0.f, 1.f, 0.f, 1.f, t);
}

float CustomFunkyEasingFunction(float t)
{
	t = GetClamped(t, 0.f, 1.f);

	//Segment 1
	if (t < 0.18f)
	{
		float s = RangeMap(t, 0.f, 0.18f, 0.f, 1.f);
		float dip = -0.08f * sinf(s * 3.14159265359f);
		float rise = 0.18f * SmoothStep3(s);
		return GetClamped(dip + rise, 0.f, 1.f);
	}

	//Segment 2
	if (t < 0.62f)
	{
		float s = RangeMap(t, 0.18f, 0.62f, 0.f, 1.f);
		float base = RangeMap(s, 0.f, 1.f, 0.18f, 0.72f);
		float wobble = 0.09f * sinf(s * 3.14159265359f * 5.f) * (1.f - s);
		return GetClamped(base + wobble, 0.f, 1.f);
	}


	float s = RangeMap(t, 0.62f, 0.82f, 0.f, 1.f);
	float settle = RangeMap(SmoothStep3(s), 0.f, 1.f, 0.72f, 1.0f);


	return GetClamped(settle, 0.f, 1.f);
}

Vec3 GetNearestPointOnPlane3D(Vec3 const& point, Plane3 const& plane)
{
	float altitude = DotProduct3D(point, plane.m_normal) - plane.m_distFromOrigin;

	return point - (plane.m_normal * altitude);
}

bool IsPointInFrontOfPlane3D(Vec3 const& point, Plane3 const& plane)
{
	float altitude = DotProduct3D(point, plane.m_normal);

	return altitude > plane.m_distFromOrigin;
}

Vec3 GetNearestPointOnOBB3D(Vec3 const& point, OBB3 const& obb)
{
	Vec3 disp = point - obb.m_center;

	float localX = DotProduct3D(disp, obb.m_iBasis);
	float localY = DotProduct3D(disp, obb.m_jBasis);
	float localZ = DotProduct3D(disp, obb.m_kBasis);

	localX = GetClamped(localX, -obb.m_halfDimensions.x, obb.m_halfDimensions.x);
	localY = GetClamped(localY, -obb.m_halfDimensions.y, obb.m_halfDimensions.y);
	localZ = GetClamped(localZ, -obb.m_halfDimensions.z, obb.m_halfDimensions.z);

	return obb.m_center + (obb.m_iBasis * localX) + (obb.m_jBasis * localY) + (obb.m_kBasis * localZ);
}

bool IsPointInsideOBB3D(Vec3 const& point, OBB3 const& obb)
{
	Vec3 disp = point - obb.m_center;

	float x = DotProduct3D(disp, obb.m_iBasis);
	float y = DotProduct3D(disp, obb.m_jBasis);
	float z = DotProduct3D(disp, obb.m_kBasis);

	return fabs(x) <= obb.m_halfDimensions.x && fabs(y) <= obb.m_halfDimensions.y && fabs(z) <= obb.m_halfDimensions.z;
}

bool DoPlane3AndSphereOverlap3D(Plane3 const& plane, Vec3 const& center, float radius)
{
	float altitude = DotProduct3D(center, plane.m_normal) - plane.m_distFromOrigin;
	return fabsf(altitude) <= radius;
}

bool DoPlane3AndAABB3Overlap3D(Plane3 const& plane, AABB3 const& box)
{
	Vec3 center = (box.m_mins + box.m_maxs) * 0.5f;
	Vec3 halfDimensions = (box.m_maxs - box.m_mins) * 0.5f;

	float centerAltitude = DotProduct3D(center, plane.m_normal) - plane.m_distFromOrigin;

	float projectedRadius =
		fabsf(plane.m_normal.x) * halfDimensions.x +
		fabsf(plane.m_normal.y) * halfDimensions.y +
		fabsf(plane.m_normal.z) * halfDimensions.z;

	return fabsf(centerAltitude) <= projectedRadius;
}

bool DoPlane3AndOBB3Overlap3D(Plane3 const& plane, OBB3 const& obb)
{
	float centerAltitude = DotProduct3D(obb.m_center, plane.m_normal) - plane.m_distFromOrigin;

	float projectedRadius =
		fabsf(DotProduct3D(obb.m_iBasis * obb.m_halfDimensions.x, plane.m_normal)) +
		fabsf(DotProduct3D(obb.m_jBasis * obb.m_halfDimensions.y, plane.m_normal)) +
		fabsf(DotProduct3D(obb.m_kBasis * obb.m_halfDimensions.z, plane.m_normal));

	return fabsf(centerAltitude) <= projectedRadius;
}

bool DoPlane3AndZCylinderOverlap3D(Plane3 const& plane, Vec2 const& centerXY, float radius, float minZ, float maxZ)
{
	Vec3 center(centerXY.x, centerXY.y, (minZ + maxZ) * 0.5f);
	float halfHeight = (maxZ - minZ) * 0.5f;

	float centerAltitude = DotProduct3D(center, plane.m_normal) - plane.m_distFromOrigin;

	float projectedRadius =
		radius * sqrtf(plane.m_normal.x * plane.m_normal.x + plane.m_normal.y * plane.m_normal.y) +
		halfHeight * fabsf(plane.m_normal.z);

	return fabsf(centerAltitude) <= projectedRadius;
}

bool DoOBB3AndSphereOverlap3D(OBB3 const& obb, Vec3 const& center, float radius)
{
	Vec3 nearest = GetNearestPointOnOBB3D(center, obb);
	return (center - nearest).GetLengthSquared() <= radius * radius;
}

bool DoOBB3AndAABB3Overlap3D(OBB3 const& obb, AABB3 const& box)
{
	Vec3 boxCorners[8];

	boxCorners[0] = Vec3(box.m_mins.x, box.m_mins.y, box.m_mins.z);
	boxCorners[1] = Vec3(box.m_maxs.x, box.m_mins.y, box.m_mins.z);
	boxCorners[2] = Vec3(box.m_mins.x, box.m_maxs.y, box.m_mins.z);
	boxCorners[3] = Vec3(box.m_maxs.x, box.m_maxs.y, box.m_mins.z);

	boxCorners[4] = Vec3(box.m_mins.x, box.m_mins.y, box.m_maxs.z);
	boxCorners[5] = Vec3(box.m_maxs.x, box.m_mins.y, box.m_maxs.z);
	boxCorners[6] = Vec3(box.m_mins.x, box.m_maxs.y, box.m_maxs.z);
	boxCorners[7] = Vec3(box.m_maxs.x, box.m_maxs.y, box.m_maxs.z);

	Vec3 firstLocal = obb.GetLocalPosForWorldPos(boxCorners[0]);

	AABB3 boxInOBBLocalSpace;
	boxInOBBLocalSpace.m_mins = firstLocal;
	boxInOBBLocalSpace.m_maxs = firstLocal;

	for (int i = 1; i < 8; ++i)
	{
		Vec3 localPos = obb.GetLocalPosForWorldPos(boxCorners[i]);

		boxInOBBLocalSpace.m_mins.x = std::min(boxInOBBLocalSpace.m_mins.x, localPos.x);
		boxInOBBLocalSpace.m_mins.y = std::min(boxInOBBLocalSpace.m_mins.y, localPos.y);
		boxInOBBLocalSpace.m_mins.z = std::min(boxInOBBLocalSpace.m_mins.z, localPos.z);

		boxInOBBLocalSpace.m_maxs.x = std::max(boxInOBBLocalSpace.m_maxs.x, localPos.x);
		boxInOBBLocalSpace.m_maxs.y = std::max(boxInOBBLocalSpace.m_maxs.y, localPos.y);
		boxInOBBLocalSpace.m_maxs.z = std::max(boxInOBBLocalSpace.m_maxs.z, localPos.z);
	}

	AABB3 obbAsLocalAABB(
		-obb.m_halfDimensions,
		obb.m_halfDimensions
	);

	return DoAABB3sOverlap3D(obbAsLocalAABB, boxInOBBLocalSpace);
}

bool DoOBB3AndZCylinderOverlap3D(OBB3 const& obb, Vec2 const& centerXY, float radius, float minZ, float maxZ)
{
	Vec3 cylinderCenter(centerXY.x, centerXY.y, (minZ + maxZ) * 0.5f);
	float halfHeight = (maxZ - minZ) * 0.5f;

	float boundingRadius = sqrtf(radius * radius + halfHeight * halfHeight);

	Vec3 nearest = GetNearestPointOnOBB3D(cylinderCenter, obb);
	return (cylinderCenter - nearest).GetLengthSquared() <= boundingRadius * boundingRadius;
}
