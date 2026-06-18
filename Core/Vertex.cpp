#include  "Engine/Core/Vertex.hpp"

Vertex::Vertex( Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords )
	:m_position( position )
	,m_color( color )
	,m_uvTexCoords( uvTexCoords )
{
}

Vertex::Vertex(Vec2 const& position, Rgba8 const& color, Vec2 const& uvTexCoords)
	:m_color(color)
	, m_uvTexCoords(uvTexCoords)
{
	m_position = Vec3(position.x, position.y, 0.f);
}
