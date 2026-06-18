#include "Game/EnemyBullet.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"

EnemyBullet::EnemyBullet(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity)
	: Entity(Owner, startingPosition, startingVelocity, 1)
{
	InitializeLocalVerts();
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	m_game = Owner;
}

void EnemyBullet::Update()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	m_velocity = GetForwardNormal() * ENEMY_BULLET_SPEED;
	m_position += (m_velocity * dt);

	if (IsOffScreen())
	{
		m_isGarbage = true;
		m_isDead = true;
	}

}

void EnemyBullet::Render() const
{
	Vertex tempWorldVerts[NUM_BULLET_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BULLET_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_BULLET_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_BULLET_VERTS, tempWorldVerts);

}

void EnemyBullet::setDead()
{
	int debrisCount = g_rng->RollRandomIntInRange(1, 3);
	m_game->SpawnNewDebrisCluster(debrisCount, m_position, -(m_velocity * 0.1f), m_cosmeticRadius * 0.2f, Rgba8(255, 255, 0, 255), 3.f, 4.f);
	m_isGarbage = true;
	m_isDead = true;
}

void EnemyBullet::InitializeLocalVerts()
{
	//Triangle A
	m_localVerts[0].m_position = Vec3(1.3f, 0.0f, 0.0f);
	m_localVerts[2].m_position = Vec3(0.0f, 1.3f, 0.0f);
	m_localVerts[1].m_position = Vec3(0.0f, -1.3f, 0.0f);

	m_localVerts[0].m_color = Rgba8(255, 255, 255, 255);
	m_localVerts[2].m_color = Rgba8(255, 255, 0, 255);
	m_localVerts[1].m_color = Rgba8(255, 255, 0, 255);



	//Triangle B
	m_localVerts[3].m_position = Vec3(0.0f, -1.3f, 0.0f);
	m_localVerts[5].m_position = Vec3(0.0f, 1.3f, 0.0f);
	m_localVerts[4].m_position = Vec3(-4.0f, 0.0f, 0.0f);

	m_localVerts[3].m_color = Rgba8(90, 57, 150);
	m_localVerts[5].m_color = Rgba8(90, 57, 150);
	m_localVerts[4].m_color = Rgba8(90, 57, 150, 0);
}