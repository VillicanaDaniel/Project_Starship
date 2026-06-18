#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Vertex_TBN.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/OBB3D.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/MathUtils.hpp"

void TransformVertexArrayXY3D(int numVerts, Vertex* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{
	Vec2 iBasis = Vec2::MakeFromPolarDegrees(rotationDegreesAboutZ, uniformScaleXY);
	Vec2 jBasis = iBasis.GetRotatedBy90Degrees();

	for (int vertIndex = 0; vertIndex < numVerts; ++vertIndex)
	{
		TransformPositionXY3D(verts[vertIndex].m_position, iBasis, jBasis, translationXY);
	}
}


void AddVertsForDisc2D(std::vector<Vertex>& verts, Vec2 const& center, float radius, Rgba8 const& color, int numSides)
{
	const float degreesPerSide = 360.f / (float)numSides;
	Vec2 prevPoint = center + Vec2::MakeFromPolarDegrees(0.f, radius);

	for (int i = 1; i <= numSides; i++)
	{
		float nextDegrees = (float)i * degreesPerSide;
		Vec2 nextPoint = center + Vec2::MakeFromPolarDegrees(nextDegrees, radius);

		Vec3 c(center.x, center.y, 0.f);
		Vec3 p(prevPoint.x, prevPoint.y, 0.f);
		Vec3 n(nextPoint.x, nextPoint.y, 0.f);

		verts.push_back(Vertex(c, color, Vec2(0.5f, 0.5f)));
		verts.push_back(Vertex(n, color, Vec2(0.5f, 0.5f)));
		verts.push_back(Vertex(p, color, Vec2(0.5f, 0.5f)));

		prevPoint = nextPoint;
	}
}

void AddVertsForAABB2(std::vector<Vertex>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 const& uvMins /*= Vec2(0.f,0.f)*/, Vec2 const& uvMaxs /*= Vec2(1.f,1.f)*/)
{
	Vec3 bl = Vec3(bounds.m_mins.x, bounds.m_mins.y, 0.f);
	Vec3 br = Vec3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);
	Vec3 tl = Vec3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);
	Vec3 tr = Vec3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	// Triangle 1
	verts.push_back(Vertex(bl, color, Vec2(uvMins.x, uvMins.y)));
	verts.push_back(Vertex(tr, color, Vec2(uvMaxs.x, uvMaxs.y)));
	verts.push_back(Vertex(br, color, Vec2(uvMaxs.x, uvMins.y)));

	// Triangle 2
	verts.push_back(Vertex(bl, color, Vec2(uvMins.x, uvMins.y)));
	verts.push_back(Vertex(tl, color, Vec2(uvMins.x, uvMaxs.y)));
	verts.push_back(Vertex(tr, color, Vec2(uvMaxs.x, uvMaxs.y)));
}

void AddVertsForOBB2D(std::vector<Vertex>& verts, OBB2 const& obb, Rgba8 const& color)
{
	Vec2 corners[4];
	obb.GetCornerPoints(corners);

	//Triangle 1
	verts.push_back(Vertex(Vec3(corners[0].x, corners[0].y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(corners[2].x, corners[2].y, 0.f), color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex(Vec3(corners[1].x, corners[1].y, 0.f), color, Vec2(0.f, 1.f)));

	//Triangle 2
	verts.push_back(Vertex(Vec3(corners[0].x, corners[0].y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(corners[3].x, corners[3].y, 0.f), color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex(Vec3(corners[2].x, corners[2].y, 0.f), color, Vec2(1.f, 1.f)));
}

void AddVertsForLineSegment2D(std::vector<Vertex>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color)
{
	Vec2 forward = end - start;
	Vec2 right(-forward.y, forward.x);
	right.Normalize();
	right *= (thickness * 0.5f);

	Vec2 corners[4];
	corners[0] = start - right;
	corners[1] = start + right;
	corners[2] = end + right;
	corners[3] = end - right;

	verts.push_back(Vertex(Vec3(corners[0].x, corners[0].y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(corners[1].x, corners[1].y, 0.f), color, Vec2(0.f, 1.f)));
	verts.push_back(Vertex(Vec3(corners[2].x, corners[2].y, 0.f), color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex(Vec3(corners[0].x, corners[0].y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(corners[2].x, corners[2].y, 0.f), color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex(Vec3(corners[3].x, corners[3].y, 0.f), color, Vec2(1.f, 0.f)));
}

void AddVertsForLineSegment3D(std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float thickness, Rgba8 const& color)
{
	Vec3 forward = end - start;
	float length = forward.GetLength();

	if (length <= 0.f)
	{
		return;
	}

	forward /= length;

	Vec3 worldUp = Vec3(0.f, 0.f, 1.f);

	if (fabsf(DotProduct3D(forward, worldUp)) > 0.99f)
	{
		worldUp = Vec3(0.f, 1.f, 0.f);
	}

	Vec3 right = CrossProduct3D(worldUp, forward).GetNormalized();
	Vec3 up = CrossProduct3D(forward, right).GetNormalized();

	float halfThickness = thickness * 0.5f;

	Vec3 r = right * halfThickness;
	Vec3 u = up * halfThickness;

	Vec3 s0 = start - r - u;
	Vec3 s1 = start + r - u;
	Vec3 s2 = start + r + u;
	Vec3 s3 = start - r + u;

	Vec3 e0 = end - r - u;
	Vec3 e1 = end + r - u;
	Vec3 e2 = end + r + u;
	Vec3 e3 = end - r + u;

	AddVertsForQuad3D(verts, s0, s1, e1, e0, color);
	AddVertsForQuad3D(verts, s1, s2, e2, e1, color);
	AddVertsForQuad3D(verts, s2, s3, e3, e2, color);
	AddVertsForQuad3D(verts, s3, s0, e0, e3, color);

	AddVertsForQuad3D(verts, s3, s2, s1, s0, color);
	AddVertsForQuad3D(verts, e0, e1, e2, e3, color);
}

void AddVertsForInfiniteLine2D(std::vector<Vertex>& verts, Vec2 const& pointOnLine, Vec2 const& direction, float thickness, Rgba8 const& color)
{
	Vec2 dir = direction;
	dir.Normalize();

	Vec2 right(-dir.y, dir.x);
	right *= (thickness * 0.5f);

	float renderLength = 10000.f; 
	Vec2 start = pointOnLine - dir * renderLength;
	Vec2 end = pointOnLine + dir * renderLength;

	Vec2 corners[4];
	corners[0] = start - right;
	corners[1] = start + right;
	corners[2] = end + right;
	corners[3] = end - right;

	verts.push_back(Vertex(Vec3(corners[0].x, corners[0].y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(corners[1].x, corners[1].y, 0.f), color, Vec2(0.f, 1.f)));
	verts.push_back(Vertex(Vec3(corners[2].x, corners[2].y, 0.f), color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex(Vec3(corners[0].x, corners[0].y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(corners[2].x, corners[2].y, 0.f), color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex(Vec3(corners[3].x, corners[3].y, 0.f), color, Vec2(1.f, 0.f)));
}

void AddVertsForTriangle2D(std::vector<Vertex>& verts, Vec2 const& p0, Vec2 const& p1, Vec2 const& p2, Rgba8 const& color)
{
	verts.push_back(Vertex(Vec3(p0.x, p0.y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(p2.x, p2.y, 0.f), color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex(Vec3(p1.x, p1.y, 0.f), color, Vec2(0.f, 0.f)));
}

void AddVertsForCapsule2D(std::vector<Vertex>& verts, Vec2 const& start, Vec2 const& end, float radius, Rgba8 const& color)
{
	int numCircleSides = 24;
	Vec2 axis = end - start;
	Vec2 dir = axis.GetNormalized();
	Vec2 perpendicular(-dir.y, dir.x);

	Vec2 r0 = start + perpendicular * radius;
	Vec2 r1 = start - perpendicular * radius;
	Vec2 r2 = end - perpendicular * radius;
	Vec2 r3 = end + perpendicular * radius;

	verts.push_back(Vertex(Vec3(r0.x, r0.y, 0.f), color, Vec2(0.5f, 0.5f)));
	verts.push_back(Vertex(Vec3(r2.x, r2.y, 0.f), color, Vec2(0.5f, 0.5f)));
	verts.push_back(Vertex(Vec3(r1.x, r1.y, 0.f), color, Vec2(0.5f, 0.5f)));

	verts.push_back(Vertex(Vec3(r0.x, r0.y, 0.f), color, Vec2(0.5f, 0.5f)));
	verts.push_back(Vertex(Vec3(r3.x, r3.y, 0.f), color, Vec2(0.5f, 0.5f)));
	verts.push_back(Vertex(Vec3(r2.x, r2.y, 0.f), color, Vec2(0.5f, 0.5f)));

	AddVertsForDisc2D(verts, start, radius, color, numCircleSides);
	AddVertsForDisc2D(verts, end, radius, color, numCircleSides);
}

void AddVertsForRing2D(std::vector<Vertex>& verts, Vec2 const& center, float innerRadius, float outerRadius, Rgba8 const& color, int numSides/* = 24*/)
{
	const float degreesPerSide = 360.f / (float)numSides;
	for (int i = 0; i < numSides; ++i)
	{
		float angle0 = i * degreesPerSide;
		float angle1 = (i + 1) * degreesPerSide;

		Vec2 p0_outer = center + Vec2::MakeFromPolarDegrees(angle0, outerRadius);
		Vec2 p1_outer = center + Vec2::MakeFromPolarDegrees(angle1, outerRadius);
		Vec2 p0_inner = center + Vec2::MakeFromPolarDegrees(angle0, innerRadius);
		Vec2 p1_inner = center + Vec2::MakeFromPolarDegrees(angle1, innerRadius);

		verts.push_back(Vertex(Vec3(p0_inner.x, p0_inner.y, 0.f), color, Vec2(0.5f, 0.5f)));
		verts.push_back(Vertex(Vec3(p1_inner.x, p1_inner.y, 0.f), color, Vec2(0.5f, 0.5f)));
		verts.push_back(Vertex(Vec3(p1_outer.x, p1_outer.y, 0.f), color, Vec2(0.5f, 0.5f)));

		verts.push_back(Vertex(Vec3(p0_inner.x, p0_inner.y, 0.f), color, Vec2(0.5f, 0.5f)));
		verts.push_back(Vertex(Vec3(p1_outer.x, p1_outer.y, 0.f), color, Vec2(0.5f, 0.5f)));
		verts.push_back(Vertex(Vec3(p0_outer.x, p0_outer.y, 0.f), color, Vec2(0.5f, 0.5f)));
	}
}

void AddVertsForArrow2D( std::vector<Vertex>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 const& color )
{
	Vec2 forward = tipPos - tailPos;
	float length = forward.GetLength();
	if (length <= 0.f)
		return;

	forward /= length;
	Vec2 right(-forward.y, forward.x);

	//Used for calculating the end of the arrow "offshoots"
	Vec2 arrowEnd = tipPos - forward * arrowSize;

	AddVertsForLineSegment2D(
		verts,
		tailPos,
		tipPos,
		lineThickness,
		color
	);

	//Arrow "head"
	float headHalfWidth = arrowSize * 0.5f;

	Vec2 leftHead = arrowEnd - right * headHalfWidth;
	Vec2 rightHead = arrowEnd + right * headHalfWidth;

	AddVertsForLineSegment2D(
		verts,
		leftHead,
		tipPos,
		lineThickness,
		color
	);

	AddVertsForLineSegment2D(
		verts,
		rightHead,
		tipPos,
		lineThickness,
		color
	);
}

void AddVertsForQuad3D(
	std::vector<Vertex>& verts,
	const Vec3& bottomLeft,
	const Vec3& bottomRight,
	const Vec3& topRight,
	const Vec3& topLeft,
	const Rgba8& color,
	const AABB2& UVs)
{
	Vec2 uvBL = UVs.m_mins;
	Vec2 uvTR = UVs.m_maxs;
	Vec2 uvBR = Vec2(uvTR.x, uvBL.y);
	Vec2 uvTL = Vec2(uvBL.x, uvTR.y);

	verts.push_back(Vertex(bottomLeft, color, uvBL));
	verts.push_back(Vertex(topRight, color, uvTR));
	verts.push_back(Vertex(bottomRight, color, uvBR));

	verts.push_back(Vertex(bottomLeft, color, uvBL));
	verts.push_back(Vertex(topLeft, color, uvTL));
	verts.push_back(Vertex(topRight, color, uvTR));
}

void AddIndexedVertsForQuad3D(
	std::vector<Vertex_TBN>& verts,
	std::vector<unsigned int>& indexes,
	Vec3 const& bl,
	Vec3 const& br,
	Vec3 const& tr,
	Vec3 const& tl,
	Rgba8 const& color,
	AABB2 const& uvs)
{
	unsigned int startIndex = (unsigned int)verts.size();

	Vec3 tangent = (br - bl).GetNormalized();
	Vec3 bitangent = (tl - bl).GetNormalized();
	Vec3 normal = CrossProduct3D(tangent, bitangent).GetNormalized();

	verts.push_back(Vertex_TBN(bl, color, Vec2(uvs.m_mins.x, uvs.m_mins.y), tangent, bitangent, normal));
	verts.push_back(Vertex_TBN(br, color, Vec2(uvs.m_maxs.x, uvs.m_mins.y), tangent, bitangent, normal));
	verts.push_back(Vertex_TBN(tr, color, Vec2(uvs.m_maxs.x, uvs.m_maxs.y), tangent, bitangent, normal));
	verts.push_back(Vertex_TBN(tl, color, Vec2(uvs.m_mins.x, uvs.m_maxs.y), tangent, bitangent, normal));

	indexes.push_back(startIndex + 0);
	indexes.push_back(startIndex + 1);
	indexes.push_back(startIndex + 2);
	indexes.push_back(startIndex + 0);
	indexes.push_back(startIndex + 2);
	indexes.push_back(startIndex + 3);
}

void TransformVertexArray3D(std::vector<Vertex>& verts, const Mat44& transform)
{
	int numVerts = (int)verts.size();

	for (int vertIndex = 0; vertIndex < numVerts; ++vertIndex)
	{
		verts[vertIndex].m_position = transform.TransformPosition3D(verts[vertIndex].m_position);
	}
}

AABB2 GetVertexBounds2D(const std::vector<Vertex>& verts)
{
	if (verts.empty())
	{
		return AABB2(Vec2(0.f, 0.f), Vec2(0.f, 0.f));
	}

	Vec2 mins(verts[0].m_position.x, verts[0].m_position.y);
	Vec2 maxs = mins;

	for (Vertex const& v : verts)
	{
		Vec2 position(v.m_position.x, v.m_position.y);

		if (position.x < mins.x) mins.x = position.x;
		if (position.y < mins.y) mins.y = position.y;
		if (position.x > maxs.x) maxs.x = position.x;
		if (position.y > maxs.y) maxs.y = position.y;
	}

	return AABB2(mins, maxs);
}

void AddVertsForCylinder3D(std::vector<Vertex>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color /*= Rgba8(255,255,255,255)*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int numSlices /*= 32*/)
{
	if (numSlices < 3)
	{
		numSlices = 3;
	}

	Vec3 axis = end - start;

	Vec3 forward = axis.GetNormalized();

	Vec3 referenceUp = Vec3(0.f, 0.f, 1.f);
	if (fabsf(forward.z) > 0.999f)
	{
		referenceUp = Vec3(0.f, 1.f, 0.f);
	}

	Vec3 right = CrossProduct3D(referenceUp, forward).GetNormalized();
	Vec3 up = CrossProduct3D(forward, right).GetNormalized();

	// --------------------------------
	// Side surface
	// --------------------------------
	for (int slice = 0; slice < numSlices; ++slice)
	{
		float frac0 = (float)slice / (float)numSlices;
		float frac1 = (float)(slice + 1) / (float)numSlices;

		float degrees0 = frac0 * 360.f;
		float degrees1 = frac1 * 360.f;

		Vec3 ringOffset0 = (right * CosDegrees(degrees0) + up * SinDegrees(degrees0)) * radius;
		Vec3 ringOffset1 = (right * CosDegrees(degrees1) + up * SinDegrees(degrees1)) * radius;

		Vec3 bottomLeft = start + ringOffset0;
		Vec3 bottomRight = start + ringOffset1;
		Vec3 topRight = end + ringOffset1;
		Vec3 topLeft = end + ringOffset0;

		float u0 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, frac0);
		float u1 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, frac1);

		AABB2 sideUVs(
			Vec2(u0, UVs.m_mins.y),
			Vec2(u1, UVs.m_maxs.y)
		);

		AddVertsForQuad3D(verts, bottomLeft, bottomRight, topRight, topLeft, color, sideUVs);
	}

	// --------------------------------
	// Bottom surface
	// --------------------------------
	Vec2 uvCenter = UVs.GetCenter();

	for (int slice = 0; slice < numSlices; ++slice)
	{
		float frac0 = (float)slice / (float)numSlices;
		float frac1 = (float)(slice + 1) / (float)numSlices;

		float degrees0 = frac0 * 360.f;
		float degrees1 = frac1 * 360.f;

		Vec3 ringOffset0 = (right * CosDegrees(degrees0) + up * SinDegrees(degrees0)) * radius;
		Vec3 ringOffset1 = (right * CosDegrees(degrees1) + up * SinDegrees(degrees1)) * radius;

		Vec3 p0 = start + ringOffset0;
		Vec3 p1 = start + ringOffset1;
		Vec3 center = start;

		Vec2 uv0 = Vec2(
			Interpolate(UVs.m_mins.x, UVs.m_maxs.x, 0.5f + 0.5f * CosDegrees(degrees0)),
			Interpolate(UVs.m_mins.y, UVs.m_maxs.y, 0.5f + 0.5f * SinDegrees(degrees0))
		);

		Vec2 uv1 = Vec2(
			Interpolate(UVs.m_mins.x, UVs.m_maxs.x, 0.5f + 0.5f * CosDegrees(degrees1)),
			Interpolate(UVs.m_mins.y, UVs.m_maxs.y, 0.5f + 0.5f * SinDegrees(degrees1))
		);

		verts.emplace_back(center, color, uvCenter);
		verts.emplace_back(p0, color, uv0);
		verts.emplace_back(p1, color, uv1);
	}

	// --------------------------------
	// Top surface
	// --------------------------------
	for (int slice = 0; slice < numSlices; ++slice)
	{
		float fraction0 = (float)slice / (float)numSlices;
		float fraction1 = (float)(slice + 1) / (float)numSlices;

		float degrees0 = fraction0 * 360.f;
		float degrees1 = fraction1 * 360.f;

		Vec3 ringOffset0 = (right * CosDegrees(degrees0) + up * SinDegrees(degrees0)) * radius;
		Vec3 ringOffset1 = (right * CosDegrees(degrees1) + up * SinDegrees(degrees1)) * radius;

		Vec3 p0 = end + ringOffset0;
		Vec3 p1 = end + ringOffset1;
		Vec3 center = end;

		Vec2 uv0 = Vec2(
			Interpolate(UVs.m_mins.x, UVs.m_maxs.x, 0.5f + 0.5f * CosDegrees(degrees0)),
			Interpolate(UVs.m_mins.y, UVs.m_maxs.y, 0.5f + 0.5f * SinDegrees(degrees0))
		);

		Vec2 uv1 = Vec2(
			Interpolate(UVs.m_mins.x, UVs.m_maxs.x, 0.5f + 0.5f * CosDegrees(degrees1)),
			Interpolate(UVs.m_mins.y, UVs.m_maxs.y, 0.5f + 0.5f * SinDegrees(degrees1))
		);

		verts.emplace_back(center, color, uvCenter);
		verts.emplace_back(p1, color, uv1);
		verts.emplace_back(p0, color, uv0);
	}
}

void AddVertsForCone3D(std::vector<Vertex>& verts, const Vec3& start, const Vec3& end, float radius, const Rgba8& color /*= Rgba8(255,255,255,255)*/, const AABB2& UVs /*= AABB2::ZERO_TO_ONE*/, int numSlices /*= 32*/)
{
	if (numSlices < 3)
	{
		numSlices = 3;
	}

	Vec3 axis = end - start;

	Vec3 forward = axis.GetNormalized();

	Vec3 referenceUp = Vec3(0.f, 0.f, 1.f);
	if (fabsf(forward.z) > 0.999f)
	{
		referenceUp = Vec3(0.f, 1.f, 0.f);
	}

	Vec3 right = CrossProduct3D(referenceUp, forward).GetNormalized();
	Vec3 up = CrossProduct3D(forward, right).GetNormalized();

	Vec2 uvCenter = UVs.GetCenter();

	// --------------------------------
	// Side surface
	// --------------------------------
	for (int slice = 0; slice < numSlices; ++slice)
	{
		float frac0 = (float)slice / (float)numSlices;
		float frac1 = (float)(slice + 1) / (float)numSlices;

		float degrees0 = frac0 * 360.f;
		float degrees1 = frac1 * 360.f;

		Vec3 ringOffset0 = (right * CosDegrees(degrees0) + up * SinDegrees(degrees0)) * radius;
		Vec3 ringOffset1 = (right * CosDegrees(degrees1) + up * SinDegrees(degrees1)) * radius;

		Vec3 base0 = start + ringOffset0;
		Vec3 base1 = start + ringOffset1;
		Vec3 tip = end;

		float u0 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, frac0);
		float u1 = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, frac1);
		float uMid = (u0 + u1) * 0.5f;

		Vec2 uv0(u0, UVs.m_mins.y);
		Vec2 uv1(u1, UVs.m_mins.y);
		Vec2 uvTip(uMid, UVs.m_maxs.y);

		verts.emplace_back(base0, color, uv0);
		verts.emplace_back(base1, color, uv1);
		verts.emplace_back(tip, color, uvTip);
	}

	// --------------------------------
	// Base cap
	// --------------------------------
	for (int slice = 0; slice < numSlices; ++slice)
	{
		float frac0 = (float)slice / (float)numSlices;
		float frac1 = (float)(slice + 1) / (float)numSlices;

		float degrees0 = frac0 * 360.f;
		float degrees1 = frac1 * 360.f;

		Vec3 ringOffset0 = (right * CosDegrees(degrees0) + up * SinDegrees(degrees0)) * radius;
		Vec3 ringOffset1 = (right * CosDegrees(degrees1) + up * SinDegrees(degrees1)) * radius;

		Vec3 p0 = start + ringOffset0;
		Vec3 p1 = start + ringOffset1;
		Vec3 center = start;

		Vec2 uv0(
			Interpolate(UVs.m_mins.x, UVs.m_maxs.x, 0.5f + 0.5f * CosDegrees(degrees0)),
			Interpolate(UVs.m_mins.y, UVs.m_maxs.y, 0.5f + 0.5f * SinDegrees(degrees0))
		);

		Vec2 uv1(
			Interpolate(UVs.m_mins.x, UVs.m_maxs.x, 0.5f + 0.5f * CosDegrees(degrees1)),
			Interpolate(UVs.m_mins.y, UVs.m_maxs.y, 0.5f + 0.5f * SinDegrees(degrees1))
		);

		verts.emplace_back(center, color, uvCenter);
		verts.emplace_back(p1, color, uv1);
		verts.emplace_back(p0, color, uv0);
	}
}

void AddVertsForArrow3D(std::vector<Vertex>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color /*= Rgba8(255,255,255,255)*/, int numSlices /*= 32*/)
{
	Vec3 axis = end - start;
	float totalLength = axis.GetLength();

	Vec3 forward = axis.GetNormalized();

	float coneLength = radius * 4.f;
	if (coneLength > totalLength * 0.5f)
	{
		coneLength = totalLength * 0.5f;
	}

	float coneRadius = radius * 2.f;

	Vec3 coneStart = end - forward * coneLength;

	AddVertsForCylinder3D(
		verts,
		start,
		coneStart,
		radius,
		color,
		AABB2::ZERO_TO_ONE,
		numSlices
	);

	AddVertsForCone3D(
		verts,
		coneStart,
		end,
		coneRadius,
		color,
		AABB2::ZERO_TO_ONE,
		numSlices
	);
}

Vec3 const MakeFromPolarRadians(float pitchRadians, float yawRadians, float length /*= 1.0f*/)
{
	float cosPitch = cosf(pitchRadians);
	float sinPitch = sinf(pitchRadians);
	float cosYaw = cosf(yawRadians);
	float sinYaw = sinf(yawRadians);

	// z is up
	float x = cosPitch * cosYaw * length;
	float y = cosPitch * sinYaw * length;
	float z = sinPitch * length;

	return Vec3(x, y, z);
}

Vec3 const MakeFromPolarDegrees(float pitchDegrees, float yawDegrees, float length /*= 1.0f*/)
{
	float cosPitch = CosDegrees(pitchDegrees);
	float sinPitch = SinDegrees(pitchDegrees);
	float cosYaw = CosDegrees(yawDegrees);
	float sinYaw = SinDegrees(yawDegrees);

	float x = cosPitch * cosYaw * length;
	float y = cosPitch * sinYaw * length;
	float z = sinPitch * length;

	return Vec3(x, y, z);
}

void AddVertsForAABB3D(std::vector<Vertex>& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE*/)
{
	Vec3 mins = bounds.m_mins;
	Vec3 maxs = bounds.m_maxs;

	Vec3 p000(mins.x, mins.y, mins.z);
	Vec3 p001(mins.x, mins.y, maxs.z);
	Vec3 p010(mins.x, maxs.y, mins.z);
	Vec3 p011(mins.x, maxs.y, maxs.z);
	Vec3 p100(maxs.x, mins.y, mins.z);
	Vec3 p101(maxs.x, mins.y, maxs.z);
	Vec3 p110(maxs.x, maxs.y, mins.z);
	Vec3 p111(maxs.x, maxs.y, maxs.z);

	// IMPORTANT:
	// Order must match your cube fix:
	// bottomLeft, bottomRight, topRight, topLeft
	// as seen from OUTSIDE

	// -X
	AddVertsForQuad3D(verts, p000, p001, p011, p010, color, UVs);

	// +X
	AddVertsForQuad3D(verts, p101, p100, p110, p111, color, UVs);

	// +Y
	AddVertsForQuad3D(verts, p011, p111, p110, p010, color, UVs);

	// -Y
	AddVertsForQuad3D(verts, p000, p100, p101, p001, color, UVs);

	// +Z
	AddVertsForQuad3D(verts, p001, p101, p111, p011, color, UVs);

	// -Z
	AddVertsForQuad3D(verts, p100, p000, p010, p110, color, UVs);
}

void AddVertsForSphere3D(std::vector<Vertex>& verts, Vec3 const& center, float radius, Rgba8 const& color /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZERO_TO_ONE*/, int numSlices /*= 32*/, int numStacks /*= 16 */)
{
	if (numSlices < 3) numSlices = 3;
	if (numStacks < 2) numStacks = 2;

	for (int stack = 0; stack < numStacks; ++stack)
	{
		float v0 = (float)stack / (float)numStacks;
		float v1 = (float)(stack + 1) / (float)numStacks;

		float pitch0 = Interpolate(-90.f, 90.f, v0);
		float pitch1 = Interpolate(-90.f, 90.f, v1);

		for (int slice = 0; slice < numSlices; ++slice)
		{
			float u0 = (float)slice / (float)numSlices;
			float u1 = (float)(slice + 1) / (float)numSlices;

			float yaw0 = u0 * 360.f;
			float yaw1 = u1 * 360.f;

			Vec3 bl = center + MakeFromPolarDegrees(pitch0, yaw0, radius);
			Vec3 br = center + MakeFromPolarDegrees(pitch0, yaw1, radius);
			Vec3 tr = center + MakeFromPolarDegrees(pitch1, yaw1, radius);
			Vec3 tl = center + MakeFromPolarDegrees(pitch1, yaw0, radius);

			float uvMinX = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, u0);
			float uvMaxX = Interpolate(UVs.m_mins.x, UVs.m_maxs.x, u1);
			float uvMinY = Interpolate(UVs.m_mins.y, UVs.m_maxs.y, v0);
			float uvMaxY = Interpolate(UVs.m_mins.y, UVs.m_maxs.y, v1);

			Vec2 uvBL(uvMinX, uvMinY);
			Vec2 uvBR(uvMaxX, uvMinY);
			Vec2 uvTR(uvMaxX, uvMaxY);
			Vec2 uvTL(uvMinX, uvMaxY);

			verts.emplace_back(bl, color, uvBL);
			verts.emplace_back(tr, color, uvTR);
			verts.emplace_back(br, color, uvBR);

			verts.emplace_back(bl, color, uvBL);
			verts.emplace_back(tl, color, uvTL);
			verts.emplace_back(tr, color, uvTR);
		}
	}
}

void AddVertsForOBB3D(std::vector<Vertex>& verts, OBB3 const& obb, Rgba8 const& color)
{
	Vec3 i = obb.m_iBasis * obb.m_halfDimensions.x;
	Vec3 j = obb.m_jBasis * obb.m_halfDimensions.y;
	Vec3 k = obb.m_kBasis * obb.m_halfDimensions.z;

	Vec3 const& c = obb.m_center;

	Vec3 bbl = c - i - j - k;
	Vec3 bbr = c + i - j - k;
	Vec3 btl = c - i + j - k;
	Vec3 btr = c + i + j - k;

	Vec3 fbl = c - i - j + k;
	Vec3 fbr = c + i - j + k;
	Vec3 ftl = c - i + j + k;
	Vec3 ftr = c + i + j + k;

	AddVertsForQuad3D(verts, bbl, btl, btr, bbr, color); // -K
	AddVertsForQuad3D(verts, fbl, fbr, ftr, ftl, color); // +K

	AddVertsForQuad3D(verts, bbl, bbr, fbr, fbl, color); // -J
	AddVertsForQuad3D(verts, btl, ftl, ftr, btr, color); // +J

	AddVertsForQuad3D(verts, bbl, fbl, ftl, btl, color); // -I
	AddVertsForQuad3D(verts, bbr, btr, ftr, fbr, color); // +I
}

void AddVertsForPlane3D(std::vector<Vertex>& verts, Plane3 const& plane, float gridHalfSize, float gridSpacing, float thickness, Rgba8 const& gridColor, Rgba8 const& normalColor, Rgba8 const& originLineColor)
{
	Vec3 normal = plane.m_normal.GetNormalized();

	Vec3 planeCenter = normal * plane.m_distFromOrigin;

	Vec3 worldUp = Vec3(0.f, 0.f, 1.f);
	if (fabsf(DotProduct3D(worldUp, normal)) > 0.95f)
	{
		worldUp = Vec3(0.f, 1.f, 0.f);
	}

	Vec3 tangent = CrossProduct3D(worldUp, normal).GetNormalized();
	Vec3 bitangent = CrossProduct3D(normal, tangent).GetNormalized();

	for (float offset = -gridHalfSize; offset <= gridHalfSize; offset += gridSpacing)
	{
		Vec3 a = planeCenter + tangent * -gridHalfSize + bitangent * offset;
		Vec3 b = planeCenter + tangent * gridHalfSize + bitangent * offset;

		AddVertsForLineSegment3D(verts, a, b, thickness, gridColor);

		Vec3 c = planeCenter + bitangent * -gridHalfSize + tangent * offset;
		Vec3 d = planeCenter + bitangent * gridHalfSize + tangent * offset;

		AddVertsForLineSegment3D(verts, c, d, thickness, gridColor);
	}

	//Line from origin to near point
	Vec3 origin = Vec3::ZERO;
	Vec3 nearPointToOrigin = planeCenter;

	AddVertsForLineSegment3D(
		verts,
		origin,
		nearPointToOrigin,
		thickness * 1.5f,
		originLineColor
	);

	// Normal indicator
	AddVertsForLineSegment3D(
		verts,
		nearPointToOrigin,
		nearPointToOrigin + normal * 0.75f,
		thickness * 2.f,
		normalColor
	);
}

void AddVertsForWireframeAABB3D(
	std::vector<Vertex>& verts,
	AABB3 const& bounds,
	float thickness,
	Rgba8 const& color)
{
	Vec3 const& mins = bounds.m_mins;
	Vec3 const& maxs = bounds.m_maxs;

	Vec3 p000(mins.x, mins.y, mins.z);
	Vec3 p100(maxs.x, mins.y, mins.z);
	Vec3 p010(mins.x, maxs.y, mins.z);
	Vec3 p110(maxs.x, maxs.y, mins.z);

	Vec3 p001(mins.x, mins.y, maxs.z);
	Vec3 p101(maxs.x, mins.y, maxs.z);
	Vec3 p011(mins.x, maxs.y, maxs.z);
	Vec3 p111(maxs.x, maxs.y, maxs.z);

	// bottom
	AddVertsForLineSegment3D(verts, p000, p100, thickness, color);
	AddVertsForLineSegment3D(verts, p100, p110, thickness, color);
	AddVertsForLineSegment3D(verts, p110, p010, thickness, color);
	AddVertsForLineSegment3D(verts, p010, p000, thickness, color);

	// top
	AddVertsForLineSegment3D(verts, p001, p101, thickness, color);
	AddVertsForLineSegment3D(verts, p101, p111, thickness, color);
	AddVertsForLineSegment3D(verts, p111, p011, thickness, color);
	AddVertsForLineSegment3D(verts, p011, p001, thickness, color);

	// verticals
	AddVertsForLineSegment3D(verts, p000, p001, thickness, color);
	AddVertsForLineSegment3D(verts, p100, p101, thickness, color);
	AddVertsForLineSegment3D(verts, p110, p111, thickness, color);
	AddVertsForLineSegment3D(verts, p010, p011, thickness, color);
}

void AddVertsForWireframeOBB3D(std::vector<Vertex>& verts, OBB3 const& obb, float thickness, Rgba8 const& color)
{
	Vec3 i = obb.m_iBasis * obb.m_halfDimensions.x;
	Vec3 j = obb.m_jBasis * obb.m_halfDimensions.y;
	Vec3 k = obb.m_kBasis * obb.m_halfDimensions.z;

	Vec3 const& c = obb.m_center;

	Vec3 p000 = c - i - j - k;
	Vec3 p001 = c - i - j + k;
	Vec3 p010 = c - i + j - k;
	Vec3 p011 = c - i + j + k;
	Vec3 p100 = c + i - j - k;
	Vec3 p101 = c + i - j + k;
	Vec3 p110 = c + i + j - k;
	Vec3 p111 = c + i + j + k;

	AddVertsForLineSegment3D(verts, p000, p100, thickness, color);
	AddVertsForLineSegment3D(verts, p100, p110, thickness, color);
	AddVertsForLineSegment3D(verts, p110, p010, thickness, color);
	AddVertsForLineSegment3D(verts, p010, p000, thickness, color);

	AddVertsForLineSegment3D(verts, p001, p101, thickness, color);
	AddVertsForLineSegment3D(verts, p101, p111, thickness, color);
	AddVertsForLineSegment3D(verts, p111, p011, thickness, color);
	AddVertsForLineSegment3D(verts, p011, p001, thickness, color);

	AddVertsForLineSegment3D(verts, p000, p001, thickness, color);
	AddVertsForLineSegment3D(verts, p100, p101, thickness, color);
	AddVertsForLineSegment3D(verts, p110, p111, thickness, color);
	AddVertsForLineSegment3D(verts, p010, p011, thickness, color);
}

void AddVertsForWireframeSphere3D(std::vector<Vertex>& verts, Vec3 const& center, float radius, float thickness, Rgba8 const& color /*= Rgba8::WHITE*/, int numSlices /*= 32 */)
{
	int const numStacks = numSlices / 2;
	float degreesPerSlice = 360.f / (float)numSlices;

	for (int stack = 1; stack < numStacks; ++stack)
	{
		float pitchDegrees = -90.f + (180.f * (float)stack / (float)numStacks);

		float z = center.z + radius * SinDegrees(pitchDegrees);
		float ringRadius = radius * CosDegrees(pitchDegrees);

		for (int slice = 0; slice < numSlices; ++slice)
		{
			float a0 = degreesPerSlice * (float)slice;
			float a1 = degreesPerSlice * (float)(slice + 1);

			Vec3 p0(
				center.x + ringRadius * CosDegrees(a0),
				center.y + ringRadius * SinDegrees(a0),
				z
			);

			Vec3 p1(
				center.x + ringRadius * CosDegrees(a1),
				center.y + ringRadius * SinDegrees(a1),
				z
			);

			AddVertsForLineSegment3D(verts, p0, p1, thickness, color);
		}
	}

	for (int slice = 0; slice < numSlices; ++slice)
	{
		float yawDegrees = degreesPerSlice * (float)slice;

		Vec3 horizontalDir(
			CosDegrees(yawDegrees),
			SinDegrees(yawDegrees),
			0.f
		);

		for (int stack = 0; stack < numStacks; ++stack)
		{
			float pitch0 = -90.f + (180.f * (float)stack / (float)numStacks);
			float pitch1 = -90.f + (180.f * (float)(stack + 1) / (float)numStacks);

			Vec3 p0 =
				center +
				horizontalDir * (radius * CosDegrees(pitch0)) +
				Vec3(0.f, 0.f, radius * SinDegrees(pitch0));

			Vec3 p1 =
				center +
				horizontalDir * (radius * CosDegrees(pitch1)) +
				Vec3(0.f, 0.f, radius * SinDegrees(pitch1));

			AddVertsForLineSegment3D(verts, p0, p1, thickness, color);
		}
	}
}

void AddVertsForWireframeZCylinder3D(std::vector<Vertex>& verts, Vec2 const& centerXY, float radius, float minZ, float maxZ, float thickness, Rgba8 const& color /*= Rgba8::WHITE*/, int numSlices /*= 32 */)
{
	float degreesPerSlice = 360.f / (float)numSlices;

	//Top and Bottom Ring
	for (int slice = 0; slice < numSlices; ++slice)
	{
		float a0 = degreesPerSlice * (float)slice;
		float a1 = degreesPerSlice * (float)(slice + 1);

		Vec3 bottom0(centerXY.x + radius * CosDegrees(a0), centerXY.y + radius * SinDegrees(a0), minZ);
		Vec3 bottom1(centerXY.x + radius * CosDegrees(a1), centerXY.y + radius * SinDegrees(a1), minZ);

		Vec3 top0(centerXY.x + radius * CosDegrees(a0), centerXY.y + radius * SinDegrees(a0), maxZ);
		Vec3 top1(centerXY.x + radius * CosDegrees(a1), centerXY.y + radius * SinDegrees(a1), maxZ);

		AddVertsForLineSegment3D(verts, bottom0, bottom1, thickness, color);
		AddVertsForLineSegment3D(verts, top0, top1, thickness, color);

		AddVertsForLineSegment3D(verts, bottom0, top0, thickness, color);

	}

	// Vertical lines
	for (int slice = 0; slice < numSlices; ++slice)
	{
		float a = degreesPerSlice * (float)slice;

		Vec3 bottom(
			centerXY.x + radius * CosDegrees(a),
			centerXY.y + radius * SinDegrees(a),
			minZ
		);

		Vec3 top(
			centerXY.x + radius * CosDegrees(a),
			centerXY.y + radius * SinDegrees(a),
			maxZ
		);

		AddVertsForLineSegment3D(verts, bottom, top, thickness, color);
	}
}

void AddVertsForIndexedAABB3D(std::vector<Vertex_TBN>& verts, std::vector<unsigned int>& indexes, AABB3 const& bounds, Rgba8 const& color)
{
	Vec3 const& mins = bounds.m_mins;
	Vec3 const& maxs = bounds.m_maxs;

	Vec3 p000(mins.x, mins.y, mins.z);
	Vec3 p100(maxs.x, mins.y, mins.z);
	Vec3 p110(maxs.x, maxs.y, mins.z);
	Vec3 p010(mins.x, maxs.y, mins.z);

	Vec3 p001(mins.x, mins.y, maxs.z);
	Vec3 p101(maxs.x, mins.y, maxs.z);
	Vec3 p111(maxs.x, maxs.y, maxs.z);
	Vec3 p011(mins.x, maxs.y, maxs.z);

	auto AddFace = [&](Vec3 const& a, Vec3 const& b, Vec3 const& c, Vec3 const& d)
		{
			unsigned int startIndex = (unsigned int)verts.size();

			Vec3 tangent = (b - a).GetNormalized();
			Vec3 bitangent = (d - a).GetNormalized();
			Vec3 normal = CrossProduct3D(tangent, bitangent).GetNormalized();

			verts.push_back(Vertex_TBN(a, color, Vec2(0.f, 0.f), tangent, bitangent, normal));
			verts.push_back(Vertex_TBN(b, color, Vec2(1.f, 0.f), tangent, bitangent, normal));
			verts.push_back(Vertex_TBN(c, color, Vec2(1.f, 1.f), tangent, bitangent, normal));
			verts.push_back(Vertex_TBN(d, color, Vec2(0.f, 1.f), tangent, bitangent, normal));

			indexes.push_back(startIndex + 0);
			indexes.push_back(startIndex + 1);
			indexes.push_back(startIndex + 2);

			indexes.push_back(startIndex + 0);
			indexes.push_back(startIndex + 2);
			indexes.push_back(startIndex + 3);
		};

	AddFace(p001, p101, p111, p011); // top
	AddFace(p100, p000, p010, p110); // bottom
	AddFace(p101, p100, p110, p111); // +X
	AddFace(p000, p001, p011, p010); // -X
	AddFace(p011, p111, p110, p010); // +Y
	AddFace(p000, p100, p101, p001); // -Y
}
