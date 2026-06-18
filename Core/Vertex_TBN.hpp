#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine\Math\Vec3.hpp"

struct Vertex_TBN
{
public:
	Vec3  m_position;
	Rgba8 m_color;
	Vec2  m_uvTexCoords;
	Vec3  m_tangent;
	Vec3  m_bitangent;
	Vec3  m_normal;

public:
	~Vertex_TBN() {}
	Vertex_TBN() {}

	explicit Vertex_TBN(
		Vec3 const& position,
		Rgba8 const& color,
		Vec2 const& uvTexCoords,
		Vec3 const& tangent = Vec3(1.f, 0.f, 0.f),
		Vec3 const& bitangent = Vec3(0.f, 1.f, 0.f),
		Vec3 const& normal = Vec3(0.f, 0.f, 1.f)
	);

	explicit Vertex_TBN(
		Vec2 const& position,
		Rgba8 const& color,
		Vec2 const& uvTexCoords,
		Vec3 const& tangent = Vec3(1.f, 0.f, 0.f),
		Vec3 const& bitangent = Vec3(0.f, 1.f, 0.f),
		Vec3 const& normal = Vec3(0.f, 0.f, 1.f)
	);
};