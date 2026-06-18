#include "Game/Asteroid.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Renderer.hpp"


//StartingVelocity is used to set orientation
Asteroid::Asteroid(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity)
	: Entity(Owner, startingPosition, startingVelocity, ASTEROID_HEALTH)
{
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;

	m_orientationDegrees = g_rng->RollRandomFloatInRange(0.1f, 360.f);
	initial_orientation =  GetForwardNormal();

	angular_velocity = g_rng->RollRandomFloatInRange(-200.f, 200.f);

	InitializeLocalVerts();
	m_game = Owner;
}

void Asteroid::Update()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	m_position += (m_velocity * dt);
	m_velocity = initial_orientation * ASTEROID_SPEED;

	m_orientationDegrees += angular_velocity * dt;
	
	WrapScreen();
}

void Asteroid::Render() const
{
	Vertex tempWorldVerts[NUM_ASTEROID_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_ASTEROID_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_ASTEROID_VERTS, tempWorldVerts);
}

void Asteroid::setDead()
{
	m_game->AddScreenShake(1.f);

	int debrisCount = g_rng->RollRandomIntInRange(3,12);
	m_game->SpawnNewDebrisCluster(debrisCount, m_position, (m_velocity * 0.1f), m_cosmeticRadius, Rgba8( 100, 100, 100, 127), 2.f, 2.f);
	m_isGarbage = true;
}

void Asteroid::TakeDamage(int damage)
{
	m_health -= damage;

	Vertex tempWorldVerts[NUM_ASTEROID_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex].m_color = Rgba8(255,255,255);
	}
	g_engine->m_render->DrawVertexArray(NUM_ASTEROID_VERTS, tempWorldVerts);
}

void Asteroid::InitializeLocalVerts()
{
	float asteroidRadii[NUM_ASTEROID_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; ++sideNum)
	{
		asteroidRadii[sideNum] = g_rng->RollRandomFloatInRange( m_physicsRadius, m_cosmeticRadius);
	}

	constexpr float degreesPerAsteroidSide = 360.f / (float) NUM_ASTEROID_SIDES;
	Vec2 asteroidLocalVertPositions[NUM_ASTEROID_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; ++sideNum)
	{
		float degrees = degreesPerAsteroidSide * (float)sideNum;
		float radius = asteroidRadii[sideNum];
		asteroidLocalVertPositions[sideNum].x = radius * CosDegrees( degrees );
		asteroidLocalVertPositions[sideNum].y = radius * SinDegrees( degrees );
	}

	for (int triNum = 0; triNum < NUM_ASTEROID_TRIS; ++triNum)
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = (triNum + 1) % NUM_ASTEROID_SIDES;
		int firstVertIndex = (triNum * 3) + 0;
		int thirdVertIndex = (triNum * 3) + 1;
		int secondVertIndex = (triNum * 3) + 2;

		Vec2 secondVertOfs = asteroidLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = asteroidLocalVertPositions[endRadiusIndex];

		m_localVerts[firstVertIndex].m_position = Vec3( 0.f, 0.f, 0.f );
		m_localVerts[secondVertIndex].m_position = Vec3( secondVertOfs.x, secondVertOfs.y, 0.f );
		m_localVerts[thirdVertIndex].m_position = Vec3( thirdVertOfs.x, thirdVertOfs.y, 0.f );
	}

	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8( 100, 100, 100);
	}
}

void Asteroid::WrapScreen()
{
	if (IsOffScreen())
	{
		if (m_position.x > WORLD_SIZE_X + m_cosmeticRadius)
		{
			m_position.x = 0.f - m_cosmeticRadius;
		}
		if (m_position.x < -m_cosmeticRadius)
		{
			m_position.x = WORLD_SIZE_X + 0.0f;
		}
		if (m_position.y > WORLD_SIZE_Y + m_cosmeticRadius)
		{
			m_position.y = 0.f - m_cosmeticRadius;
		}
		if (m_position.y < -m_cosmeticRadius)
		{
			m_position.y = WORLD_SIZE_Y + 0.0f;
		}
	}
}

