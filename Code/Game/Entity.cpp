#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"

Entity::Entity(Game* owner, Vec2 const& startingPosition, Vec2 const& startingVelocity, int health)
	:m_game( owner)
	,m_position( startingPosition )
	,m_velocity( startingVelocity )
	,m_health( health )
{
}


void Entity::RenderDebug() const
{
	//Draw Rings
	DebugDrawRing(m_position, m_cosmeticRadius, 0.2f, Rgba8(255, 0, 255));
	DebugDrawRing(m_position, m_physicsRadius, 0.2f, Rgba8(0, 255, 255));

	//Draw forward and perpendicular line to cosmetic radius
	DebugDrawLine(m_position, m_position + GetForwardNormal() * m_cosmeticRadius, 0.2f, Rgba8(255, 0, 0));
	DebugDrawLine(m_position, m_position + (GetForwardNormal().GetRotatedBy90Degrees()) * m_cosmeticRadius, 0.2f, Rgba8(0, 255, 0));

	//Draw Velocity Line
	DebugDrawLine(m_position, m_position + m_velocity, 0.2f, Rgba8(255,255,0));
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees));
}

bool Entity::IsOffScreen() const
{
	if (m_position.x > WORLD_SIZE_X + m_cosmeticRadius || m_position.x < - m_cosmeticRadius)
		return true;
	else if (m_position.y > WORLD_SIZE_Y + m_cosmeticRadius || m_position.y < -m_cosmeticRadius)
		return true;
	else
		return false;
}

void Entity::TakeDamage(int damage)
{
	m_health -= damage;
}

void Entity::setDead() 
{
	m_isDead = true;
}

