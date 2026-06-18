#include "Engine/Core/Vertex_TBN.hpp"

Vertex_TBN::Vertex_TBN(
	Vec3 const& position,
	Rgba8 const& color,
	Vec2 const& uvTexCoords,
	Vec3 const& tangent,
	Vec3 const& bitangent,
	Vec3 const& normal)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
	, m_tangent(tangent)
	, m_bitangent(bitangent)
	, m_normal(normal)
{
}

Vertex_TBN::Vertex_TBN(
	Vec2 const& position,
	Rgba8 const& color,
	Vec2 const& uvTexCoords,
	Vec3 const& tangent,
	Vec3 const& bitangent,
	Vec3 const& normal)
	: m_position(Vec3(position.x, position.y, 0.f))
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
	, m_tangent(tangent)
	, m_bitangent(bitangent)
	, m_normal(normal)
{
}