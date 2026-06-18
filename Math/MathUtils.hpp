#pragma once
#include "Engine/Math/Vec2.hpp"
// Forward type declarations

enum class BillboardType
{
	NONE = -1,
	WORLD_UP_FACING,
	WORLD_UP_OPPOSING,
	FULL_FACING,
	FULL_OPPOSING,
	COUNT
};

struct Vec3;
struct Vec4;
struct IntVec2;
struct OBB2;
struct AABB2;
struct Mat44;
struct AABB3;
struct OBB3;
struct Plane3;

// Angle utilities
float ConvertDegreesToRadians( float degrees );
float ConvertRadiansToDegrees( float radians );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float Atan2Degrees( float y, float x );


// Basic 2D & 3D utilities
float GetDistance2D( Vec2 const& positionA, Vec2 const& positionB ); 
float GetDistanceSquared2D( Vec2 const& positionA, Vec2 const& positionB ); 
float GetDistance3D( Vec3 const& positionA, Vec3 const& positionB ); 
float GetDistanceSquared3D( Vec3 const& positionA, Vec3 const& positionB ); 
float GetDistanceXY3D( Vec3 const& positionA, Vec3 const& positionB ); 
float GetDistanceXYSquared3D( Vec3 const& positionA, Vec3 const& positionB );

// Geometric query utilities
bool DoDiscsOverlap( Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB ); 
bool DoSpheresOverlap( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB );

// Transform utilities
void TransformPosition2D( Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation ); 
void TransformPositionXY3D( Vec3& posToTransform, float xyScale, float zRotationDegrees, Vec2 const& xyTranslation );

void TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
void TransformPositionXY3D(Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);

//Assignment 3, not reformatted yet
float Interpolate( float start, float end, float tWeightOfEnd);
Vec2 InterpolateVec2(Vec2 const& start, Vec2 const& end, float t);
float GetFractionWithinRange( float value, float start, float end);
float RangeMap( float inValue, float inRangeStart, float inRangeEnd, float outRangeStart, float outRangeEnd);
float RangeMapClamped( float inValue, float inStart, float inEnd, float outStart, float outEnd);
int RoundDownToInt( float value);

float GetClamped(float value, float minValue, float maxValue);
float GetClampedZeroToOne(float value);

float DotProduct2D( Vec2 const& a, Vec2 const& b );

float GetShortestAngularDispDegrees( float startDegrees, float endDegrees);
float GetTurnedTowardDegrees( float currentDegrees, float goalDegrees, float maxDeltaDegrees);

//Assignment 4, not reformatted yet
int GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB );
float GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );
Vec2 GetProjectedVector2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );

bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius );
bool IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorFwdDegrees, float sectorApertureDegrees, float sectorRadius );
bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorFwdNormal, float sectorApertureDegrees, float sectorRadius );
Vec2 GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius );
bool PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint );
bool PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius );
bool PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius );
bool PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox );

float GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b );

//Assigment 5, not reformatted yet
bool IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box);
bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& box);
bool IsPointInsideTriangle2D(Vec2 const& point, Vec2 const& vertexA, Vec2 const& vertexB, Vec2 const& vertexC);
bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);

Vec2 GetNearestPointOnAABB2D(Vec2 const& point, AABB2 const& box);
Vec2 GetNearestPointOnOBB2D(Vec2 const& point, OBB2 const& box);
Vec2 GetNearestPointOnLineSegment2D(Vec2 const& point, Vec2 const& start, Vec2 const& end);
Vec2 GetNearestPointOnInfiniteLine2D(Vec2 const& point, Vec2 const& linePoint, Vec2 const& lineDir);
Vec2 GetNearestPointOnTriangle2D(Vec2 const& point, Vec2 const& a, Vec2 const& b, Vec2 const& c);
Vec2 GetNearestPointOnCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);

//MP2, Assignment 1
float DotProduct3D(Vec3 const& a, Vec3 const& b);
float DotProduct4D(Vec4 const& a, Vec4 const& b);

float CrossProduct2D(Vec2 const& a, Vec2 const& b);
Vec3  CrossProduct3D(Vec3 const& a, Vec3 const& b);

float NormalizeByte(unsigned char byteValue);
unsigned char DenormalizeByte(float zeroToOne);

Mat44 GetBillboardTransform(
	BillboardType billboardType,
	Mat44 const& targetTransform,
	Vec3 const& billboardPosition,
	Vec2 const& billboardScale = Vec2(1.f, 1.f));

//MP2, Assignment 4
bool DoAABB3sOverlap3D(AABB3 const& first, AABB3 const& second);
bool DoSpheresOverlap3D(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);
bool DoZCylindersOverlap3D(Vec2 const& centerA, float radiusA, float minZA, float maxZA,
	Vec2 const& centerB, float radiusB, float minZB, float maxZB);
bool DoSphereAndAABB3Overlap3D(Vec3 const& sphereCenter, float sphereRadius, AABB3 const& box);
bool DoZCylinderAndAABB3Overlap3D(Vec2 const& cylinderCenterXY, float cylinderRadius, float cylinderMinZ, float cylinderMaxZ, AABB3 const& box);
bool DoZCylinderAndSphereOverlap3D(Vec2 const& cylinderCenterXY, float cylinderRadius, float cylinderMinZ, float cylinderMaxZ,
	Vec3 const& sphereCenter, float sphereRadius);

//MP, Assignment 5
float ComputeCubicBezier1D(float A, float B, float C, float D, float t);
float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t);

float SmoothStart2(float t);
float SmoothStart3(float t);
float SmoothStart4(float t);
float SmoothStart5(float t);
float SmoothStart6(float t);

float SmoothStop2(float t);
float SmoothStop3(float t);
float SmoothStop4(float t);
float SmoothStop5(float t);
float SmoothStop6(float t);

float SmoothStep3(float t);
float SmoothStep5(float t);

float Hesitate3(float t);
float Hesitate5(float t);

float CustomFunkyEasingFunction(float t);

//MP Assignment 7
Vec3 GetNearestPointOnPlane3D(Vec3 const& point, Plane3 const& plane);
bool IsPointInFrontOfPlane3D(Vec3 const& point, Plane3 const& plane);
Vec3 GetNearestPointOnOBB3D(Vec3 const& point, OBB3 const& obb);
bool IsPointInsideOBB3D(Vec3 const& point, OBB3 const& obb);

bool DoPlane3AndSphereOverlap3D(Plane3 const& plane, Vec3 const& center, float radius);
bool DoPlane3AndAABB3Overlap3D(Plane3 const& plane, AABB3 const& box);
bool DoPlane3AndOBB3Overlap3D(Plane3 const& plane, OBB3 const& obb);
bool DoPlane3AndZCylinderOverlap3D(Plane3 const& plane, Vec2 const& centerXY, float radius, float minZ, float maxZ);

bool DoOBB3AndAABB3Overlap3D(OBB3 const& obb, AABB3 const& box);
bool DoOBB3AndSphereOverlap3D(OBB3 const& obb, Vec3 const& center, float radius);
bool DoOBB3AndZCylinderOverlap3D(OBB3 const& obb, Vec2 const& centerXY, float radius, float minZ, float maxZ);

