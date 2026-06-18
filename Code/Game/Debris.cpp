#include "Game/Debris.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Renderer.hpp"


Debris::Debris(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity, float radius, Rgba8 color)
	: Entity(Owner, startingPosition, startingVelocity, 3)
{
	m_cosmeticRadius = radius * g_rng->RollRandomFloatInRange(0.7f, 1.3f);
	m_physicsRadius = m_cosmeticRadius / 4;

	debris_color = color;
	angular_velocity = g_rng->RollRandomFloatInRange(-200.f, 200.f);

	m_game = Owner;

	InitializeLocalVerts();
}

void Debris::Update()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	m_position += (m_velocity * dt);

	m_orientationDegrees += angular_velocity * dt;
	
	if( m_timeoutTimer > 0)
	{ 
		m_timeoutTimer -= dt;
		alphaValue = RangeMapClamped(m_timeoutTimer, 0.0f, DEBRIS_LIFETIME_SECONDS, 0.f, 127.0f);
	}
	else if(m_timeoutTimer <= 0)
	{
		m_isDead = true;
		m_isGarbage = true;
	}

	if (IsOffScreen())
	{
		m_isGarbage = true;
		m_isDead = true;
	}
}

void Debris::Render() const
{
	Vertex tempWorldVerts[16*3];
	for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
		tempWorldVerts[vertIndex].m_color.a = static_cast<unsigned char>(alphaValue);
	}

	TransformVertexArrayXY3D(NUM_DEBRIS_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_DEBRIS_VERTS, tempWorldVerts);
}


void Debris::InitializeLocalVerts()
{
	float asteroidRadii[NUM_DEBRIS_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_DEBRIS_SIDES; ++sideNum)
	{
		asteroidRadii[sideNum] = g_rng->RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
	}

	constexpr float degreesPerAsteroidSide = 360.f / (float)NUM_DEBRIS_SIDES;
	Vec2 asteroidLocalVertPositions[NUM_DEBRIS_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_DEBRIS_SIDES; ++sideNum)
	{
		float degrees = degreesPerAsteroidSide * (float)sideNum;
		float radius = asteroidRadii[sideNum];
		asteroidLocalVertPositions[sideNum].x = radius * CosDegrees(degrees);
		asteroidLocalVertPositions[sideNum].y = radius * SinDegrees(degrees);
	}

	for (int triNum = 0; triNum < NUM_DEBRIS_TRIS; ++triNum)
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = (triNum + 1) % NUM_DEBRIS_SIDES;
		int firstVertIndex = (triNum * 3) + 0;
		int secondVertIndex = (triNum * 3) + 1;
		int thirdVertIndex = (triNum * 3) + 2;

		Vec2 secondVertOfs = asteroidLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = asteroidLocalVertPositions[endRadiusIndex];

		m_localVerts[firstVertIndex].m_position = Vec3(0.f, 0.f, 0.f);
		m_localVerts[thirdVertIndex].m_position = Vec3(secondVertOfs.x, secondVertOfs.y, 0.f);
		m_localVerts[secondVertIndex].m_position = Vec3(thirdVertOfs.x, thirdVertOfs.y, 0.f);
	}

	for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = debris_color;
	}
}
