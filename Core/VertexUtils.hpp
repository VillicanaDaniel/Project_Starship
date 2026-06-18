#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>

struct Vec2;
struct Vec3;
struct AABB3;
struct Vertex;
struct Vertex_TBN;
struct OBB2;
struct Mat44;
struct OBB3;
struct Plane3;

void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);
void AddVertsForDisc2D(std::vector<Vertex>& verts, Vec2 const& center, float radius, Rgba8 const& color, int numSides = 32);
void AddVertsForAABB2(std::vector<Vertex>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 const& uvMins = Vec2(0.f,0.f), Vec2 const& uvMaxs = Vec2(1.f,1.f));
void AddVertsForOBB2D(std::vector<Vertex>& verts, OBB2 const& obb, Rgba8 const& color);
void AddVertsForLineSegment2D(std::vector<Vertex>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color);
void AddVertsForLineSegment3D(std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color);
void AddVertsForInfiniteLine2D(std::vector<Vertex>& verts, Vec2 const& pointOnLine, Vec2 const& direction, float thickness, Rgba8 const& color);
void AddVertsForTriangle2D(std::vector<Vertex>& verts, Vec2 const& p0, Vec2 const& p1, Vec2 const& p2, Rgba8 const& color);
void AddVertsForCapsule2D(std::vector<Vertex>& verts, Vec2 const& start, Vec2 const& end, float radius, Rgba8 const& color);
void AddVertsForRing2D(std::vector<Vertex>& verts, Vec2 const& center, float innerRadius, float outerRadius, Rgba8 const& color, int numSides = 24);
void AddVertsForArrow2D( std::vector<Vertex>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 const& color );
void AddVertsForQuad3D(std::vector<Vertex>& verts, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& topRight, const Vec3& topLeft, const Rgba8& color, const AABB2& UVs = AABB2::ZERO_TO_ONE);
void AddIndexedVertsForQuad3D(
	std::vector<Vertex_TBN>& verts,
	std::vector<unsigned int>& indexes,
	Vec3 const& bl,
	Vec3 const& br,
	Vec3 const& tr,
	Vec3 const& tl,
	Rgba8 const& color,
	AABB2 const& uvs);

void TransformVertexArray3D(std::vector<Vertex>& verts, const Mat44& transform);
AABB2 GetVertexBounds2D(const std::vector<Vertex>& verts);
void AddVertsForCylinder3D(std::vector<Vertex>& verts, const Vec3& start, const Vec3& end, float radius,
							const Rgba8& color = Rgba8(255,255,255,255), const AABB2& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32);
void AddVertsForCone3D(std::vector<Vertex>& verts, const Vec3& start, const Vec3& end, float radius,
						const Rgba8& color = Rgba8(255,255,255,255), const AABB2& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32);
void AddVertsForArrow3D(std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float radius,
						Rgba8 const& color = Rgba8(255,255,255,255), int numSlices = 32);

Vec3 const MakeFromPolarRadians(float pitchRadians, float yawRadians, float length = 1.0f);
Vec3 const MakeFromPolarDegrees(float pitchDegrees, float yawDegrees, float length = 1.0f);

void AddVertsForAABB3D(std::vector<Vertex>& verts, AABB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForSphere3D(std::vector<Vertex>& verts, Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE, 
						 AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32, int numStacks = 16);
void AddVertsForOBB3D(std::vector<Vertex>& verts, OBB3 const& obb, Rgba8 const& color);
void AddVertsForPlane3D(std::vector<Vertex>& verts, Plane3 const& plane, float gridHalfSize, float gridSpacing,
						float thickness, Rgba8 const& gridColor, Rgba8 const& normalColor, Rgba8 const& originLineColor);

void AddVertsForWireframeAABB3D(std::vector<Vertex>& verts, AABB3 const& bounds, float thickness, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForWireframeOBB3D(std::vector<Vertex>& verts, OBB3 const& obb, float thickness, Rgba8 const& color);
void AddVertsForWireframeSphere3D(std::vector<Vertex>& verts, Vec3 const& center, float radius, float thickness, Rgba8 const& color = Rgba8::WHITE, int numSlices = 32);
void AddVertsForWireframeZCylinder3D(std::vector<Vertex>& verts, Vec2 const& centerXY, float radius, float minZ, float maxZ, float thickness, 
									 Rgba8 const& color = Rgba8::WHITE, int numSlices = 32);

//Indexed Geometry
void AddVertsForIndexedAABB3D(
	std::vector<Vertex_TBN>& verts,
	std::vector<unsigned int>& indexes,
	AABB3 const& bounds,
	Rgba8 const& color
);