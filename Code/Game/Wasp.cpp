#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Wasp.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Renderer.hpp"

Wasp::Wasp(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity)
	: Entity(Owner, startingPosition, startingVelocity, WASP_HEALTH)
{
	m_physicsRadius = WASP_PHYSICS_RADIUS;
	m_cosmeticRadius = WASP_COSMETIC_RADIUS;
	isCloseToPlayer = false;

	InitializeLocalVerts();

	m_game = Owner;
}


void Wasp::Update()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();

	Vec2 playerPos = m_game->m_ship1->m_position;
	Vec2 forwardVec = GetForwardNormal();
	float distanceFromPlayer = GetDistance2D(m_position, playerPos);

	if (distanceFromPlayer <= 20.f)
	{
		isCloseToPlayer = true;
	}
	else if(distanceFromPlayer >= 50.f)
	{
		isCloseToPlayer = false;
	}

	if (isCloseToPlayer)
	{
		Vec2 dispToPlayer = m_game->m_ship1->m_position - m_position;
		m_orientationDegrees = dispToPlayer.GetOrientationDegrees() + 80.f;
		m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, (WASP_SPEED * 0.5f) + (distanceFromPlayer * 2.5f));
	}
	else if(!isCloseToPlayer)
	{
		m_velocity.x = forwardVec.x * WASP_SPEED;
		m_velocity.y = forwardVec.y * WASP_SPEED;
	}
	m_position += (m_velocity * dt);

	// Wing animation timer
	if (m_wingTimer > 0.f)
	{
		m_wingTimer -= dt;
	}
	else
	{
		m_wingTimer = WASP_WING_TIMER;
	}
}

void Wasp::Render() const
{
		Vertex tempWorldVerts[15];
		for (int vertIndex = 0; vertIndex < 15; ++vertIndex)
		{
			tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
		}

		if (m_wingTimer <= 1)
		{
			//Wing 1
			tempWorldVerts[0].m_position = Vec3(-1.f, 2.f, 0.f);
			tempWorldVerts[1].m_position = Vec3(0.f, 2.f, 0.f);
			tempWorldVerts[2].m_position = Vec3(0.f, 0.f, 0.f);

			//Wing 2
			tempWorldVerts[3].m_position = Vec3(-1.f, -2.f, 0.f);
			tempWorldVerts[4].m_position = Vec3(0.f, -2.f, 0.f);
			tempWorldVerts[5].m_position = Vec3(0.f, 0.f, 0.f);
		}
		TransformVertexArrayXY3D(15, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
		g_engine->m_render->DrawVertexArray(15, tempWorldVerts);
}

void Wasp::setDead()
{
	m_game->AddScreenShake(1.f);

	int debrisCount = g_rng->RollRandomIntInRange(3,12);
	m_game->SpawnNewDebrisCluster(debrisCount, m_position, (m_velocity*0.05f), m_cosmeticRadius, Rgba8(255, 255, 0), 3.f, 4.f);
	m_isGarbage = true;
}

void Wasp::InitializeLocalVerts()
{
	// Wing 1
	m_localVerts[0].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[2].m_position = Vec3(0.f, 0.f, 0.f);
	m_localVerts[1].m_position = Vec3(1.f, 2.f, 0.f);

	// Wing 2
	m_localVerts[3].m_position = Vec3(0.f, -2.f, 0.f);
	m_localVerts[4].m_position = Vec3(1.f, -2.f, 0.f);
	m_localVerts[5].m_position = Vec3(0.f, 0.f, 0.f);

	// Body
	m_localVerts[6].m_position = Vec3(0.f, -0.5f, 0.f);
	m_localVerts[7].m_position = Vec3(-2.f, 0.f, 0.f);
	m_localVerts[8].m_position = Vec3(0.f, 0.5f, 0.f);

	// Head
	m_localVerts[9].m_position = Vec3(0.f, -0.5f, 0.f);
	m_localVerts[11].m_position = Vec3(1.f, 0.f, 0.f);
	m_localVerts[10].m_position = Vec3(0.f, 0.5f, 0.f);

	// Stinger
	m_localVerts[12].m_position = Vec3(-1.f, -0.5f, 0.f);
	m_localVerts[13].m_position = Vec3(-2.f, 0.f, 0.f);
	m_localVerts[14].m_position = Vec3(-1.f, 0.5f, 0.f);

	for (int vertIndex = 0; vertIndex < 15; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(255, 255, 0);
		m_localVerts[vertIndex].m_uvTexCoords = Vec2(0.f, 0.f);
	}

	m_localVerts[12].m_color = Rgba8(255, 255, 255);
	m_localVerts[13].m_color = Rgba8(255, 255, 255);
	m_localVerts[14].m_color = Rgba8(255, 255, 255);
}

