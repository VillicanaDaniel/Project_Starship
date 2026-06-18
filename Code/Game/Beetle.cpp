#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Beetle.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Renderer.hpp"


//StartingVelocity is used to set orientation
Beetle::Beetle(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity)
	: Entity(Owner, startingPosition, startingVelocity, BEETLE_HEALTH)
{
	m_physicsRadius = BEETLE_PHYSICS_RADIUS;
	m_cosmeticRadius = BEETLE_COSMETIC_RADIUS;

	InitializeLocalVerts();

	m_game = Owner;
}

void Beetle::Update()
{	
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	if (!(m_game->m_ship1->m_isDead))
	{
		m_orientationDegrees = Atan2Degrees((WORLD_SIZE_Y- m_position.y), (-m_position.x));
	}
	m_velocity = GetForwardNormal() * BEETLE_SPEED;
	m_position += (m_velocity * dt);
}

void Beetle::Render() const
{
	Vertex tempWorldVerts[21];
	for (int vertIndex = 0; vertIndex < 21; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(21, tempWorldVerts, 0.75f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(21, tempWorldVerts);
}

void Beetle::setDead()
{
	m_game->AddScreenShake(1.f);

	int debrisCount = g_rng->RollRandomIntInRange(3,12);
	m_game->SpawnNewDebrisCluster(debrisCount, m_position, (m_velocity * 0.1f), m_cosmeticRadius, Rgba8(0,255,0), 2.f, 2.f);
	m_isGarbage = true;
}

void Beetle::InitializeLocalVerts()
{
	// --- HEAD ---
	m_localVerts[0].m_position = Vec3(-6.f, 2.f, 0.f);
	m_localVerts[1].m_position = Vec3(-6.f, -2.f, 0.f);
	m_localVerts[2].m_position = Vec3(-10.f, 0.f, 0.f);

	// --- PRONOTUM (Shell behind head) ---
	m_localVerts[3].m_position = Vec3(-4.f, 3.f, 0.f);
	m_localVerts[4].m_position = Vec3(-6.f, 0.f, 0.f);
	m_localVerts[5].m_position = Vec3(-4.f, -3.f, 0.f);

	m_localVerts[6].m_position = Vec3(-2.f, 4.f, 0.f);
	m_localVerts[7].m_position = Vec3(-2.f, -4.f, 0.f);
	m_localVerts[8].m_position = Vec3(-4.f, 0.f, 0.f);

	// --- BODY / ELYTRA ---
	m_localVerts[9].m_position = Vec3(-1.f, 5.f, 0.f);
	m_localVerts[10].m_position = Vec3(-1.f, -5.f, 0.f);
	m_localVerts[11].m_position = Vec3(-4.f, 0.f, 0.f);

	m_localVerts[12].m_position = Vec3(2.f, 6.f, 0.f);
	m_localVerts[13].m_position = Vec3(2.f, -6.f, 0.f);
	m_localVerts[14].m_position = Vec3(0.f, 0.f, 0.f);

	// --- LEGS (Top row) ---
	m_localVerts[15].m_position = Vec3(2.f, 7.f, 0.f);
	m_localVerts[16].m_position = Vec3(2.f, 4.f, 0.f);
	m_localVerts[17].m_position = Vec3(6.f, 7.f, 0.f);

	// --- LEGS (Bottom row) ---
	m_localVerts[18].m_position = Vec3(2.f, -4.f, 0.f);
	m_localVerts[19].m_position = Vec3(2.f, -7.f, 0.f);
	m_localVerts[20].m_position = Vec3(6.f, -7.f, 0.f);

	// ===================== COLORS =====================
	for (int i = 0; i < 21; i++)
	{
		m_localVerts[i].m_color = Rgba8(0, 255, 0, 255);
		m_localVerts[i].m_uvTexCoords = Vec2(0.f, 0.f);
	}

	// Slight variations for contrast:
	for (int i = 12; i < 21; i++) // legs/body rear
	{
		m_localVerts[i].m_color = Rgba8(0, 150, 0, 255);
	}
}