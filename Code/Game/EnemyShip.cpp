#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/EnemyShip.hpp"
#include "Game/EnemyBullet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Renderer.hpp"

EnemyShip::EnemyShip(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity)
	: Entity(Owner, startingPosition, startingVelocity, ENEMY_SHIP_HEALTH)
{
	m_physicsRadius = ENEMY_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = ENEMY_SHIP_COSMETIC_RADIUS;

	InitializeLocalVerts();

	m_game = Owner;
}


void EnemyShip::Update()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	Vec2 playerPos = m_game->m_ship1->m_position;
	float distanceFromPlayer = GetDistance2D(m_position, playerPos);

	if (distanceFromPlayer < ENEMY_SHIP_SHOOT_DISTANCE && !(m_game->m_ship1->m_isDead))
	{
		float goalDegrees = Atan2Degrees((m_game->m_ship1->m_position.y - m_position.y), (m_game->m_ship1->m_position.x - m_position.x));
		float maxTurnSpeed = ENEMY_SHIP_TURN_SPEED;
		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, goalDegrees, maxTurnSpeed * dt);

		if (m_bulletTimer > 0)
		{
			m_bulletTimer -= dt;
			m_velocity = GetForwardNormal() * 5.f;
		}
		else
		{
			g_engine->m_audio->StartSound(m_game->m_pewSound);
			SpawnNewBullet();
			m_bulletTimer = ENEMY_SHIP_FIRE_SPEED;
		}
	}
	else if(!(m_game->m_ship1->m_isDead))
	{
		m_orientationDegrees = Atan2Degrees((m_game->m_ship1->m_position.y - m_position.y), (m_game->m_ship1->m_position.x - m_position.x));

		m_velocity = GetForwardNormal() * 30;
	}

	m_position += (m_velocity * dt);
}

void EnemyShip::Render() const
{
	Vertex tempWorldVerts[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_SHIP_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_SHIP_VERTS, tempWorldVerts);
}

void EnemyShip::setDead()
{
	m_game->AddScreenShake(1.f);
	
	int debrisCount = g_rng->RollRandomIntInRange(3, 12);
	m_game->SpawnNewDebrisCluster(debrisCount, m_position, (m_velocity * 0.5f), m_cosmeticRadius, Rgba8(255, 50, 100, 255), 3.f, 4.f);
	m_isGarbage = true;
}

void EnemyShip::InitializeLocalVerts()
{
	//HULL
	m_localVerts[0].m_position = Vec3(3.f, 0.f, 0.f);
	m_localVerts[1].m_position = Vec3(0.f, -1.f, 0.f);
	m_localVerts[2].m_position = Vec3(0.f, 1.f, 0.f);

	m_localVerts[3].m_position = Vec3(-2.f, 0.f, 0.f);
	m_localVerts[4].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[5].m_position = Vec3(0.f, -1.f, 0.f);

	m_localVerts[0].m_color = Rgba8(255, 50, 100, 255);
	m_localVerts[1].m_color = Rgba8(255, 50, 100, 255);
	m_localVerts[2].m_color = Rgba8(255, 50, 100, 255);

	m_localVerts[3].m_color = Rgba8(255, 50, 100, 255);
	m_localVerts[4].m_color = Rgba8(255, 50, 100, 255);
	m_localVerts[5].m_color = Rgba8(255, 50, 100, 255);


	//COCKPIT
	m_localVerts[6].m_position = Vec3(1.f, 0.f, 0.f);
	m_localVerts[7].m_position = Vec3(0.f, -0.5f, 0.f);
	m_localVerts[8].m_position = Vec3(0.f, 0.5f, 0.f);

	m_localVerts[9].m_position = Vec3(-1.f, 0.f, 0.f);
	m_localVerts[10].m_position = Vec3(0.f, 0.5f, 0.f);
	m_localVerts[11].m_position = Vec3(0.f, -0.5f, 0.f);

	m_localVerts[6].m_color = Rgba8(0, 0, 0, 255);
	m_localVerts[7].m_color = Rgba8(0, 0, 0, 255);
	m_localVerts[8].m_color = Rgba8(0, 0, 0, 255);

	m_localVerts[9].m_color = Rgba8(0, 0, 0, 255);
	m_localVerts[10].m_color = Rgba8(0, 0, 0, 255);
	m_localVerts[11].m_color = Rgba8(0, 0, 0, 255);

	//LEFT BLUE THING LOL
	m_localVerts[12].m_position = Vec3(-1.f, 1.5f, 0.f);
	m_localVerts[13].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[14].m_position = Vec3(0.f, 2.f, 0.f);

	m_localVerts[15].m_position = Vec3(1.f, 1.5f, 0.f);
	m_localVerts[16].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[17].m_position = Vec3(0.f, 1.f, 0.f);

	m_localVerts[12].m_color = Rgba8(90, 57, 97);
	m_localVerts[13].m_color = Rgba8(90, 57, 97);
	m_localVerts[14].m_color = Rgba8(90, 57, 97);

	m_localVerts[15].m_color = Rgba8(90, 57, 97);
	m_localVerts[16].m_color = Rgba8(90, 57, 97);
	m_localVerts[17].m_color = Rgba8(90, 57, 97);

	//LEFT WING
	m_localVerts[18].m_position = Vec3(-1.f, 5.f, 0.f);
	m_localVerts[19].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[20].m_position = Vec3(-1.f, 1.5f, 0.f);

	m_localVerts[18].m_color = Rgba8(255, 50, 100, 255);
	m_localVerts[19].m_color = Rgba8(255, 50, 100, 255);
	m_localVerts[20].m_color = Rgba8(255, 50, 100, 255);

	//RIGHT BLUE THING LOL
	m_localVerts[21].m_position = Vec3(-1.f, -1.5f, 0.f);
	m_localVerts[22].m_position = Vec3(0.f, -2.f, 0.f);
	m_localVerts[23].m_position = Vec3(0.f, -1.f, 0.f);

	m_localVerts[24].m_position = Vec3(1.f, -1.5f, 0.f);
	m_localVerts[25].m_position = Vec3(0.f, -1.f, 0.f);
	m_localVerts[26].m_position = Vec3(0.f, -2.f, 0.f);

	m_localVerts[21].m_color = Rgba8(90, 57, 97);
	m_localVerts[22].m_color = Rgba8(90, 57, 97);
	m_localVerts[23].m_color = Rgba8(90, 57, 97);

	m_localVerts[24].m_color = Rgba8(90, 57, 97);
	m_localVerts[25].m_color = Rgba8(90, 57, 97);
	m_localVerts[26].m_color = Rgba8(90, 57, 97);

	//RIGHT WING
	m_localVerts[27].m_position = Vec3(-1.f, -5.f, 0.f);
	m_localVerts[28].m_position = Vec3(-1.f, -1.5f, 0.f);
	m_localVerts[29].m_position = Vec3(0.f, -2.f, 0.f);

	m_localVerts[27].m_color = Rgba8(255, 50, 100, 255);
	m_localVerts[28].m_color = Rgba8(255, 50, 100, 255);
	m_localVerts[29].m_color = Rgba8(255, 50, 100, 255);

	//Thrust Triangle
	m_localVerts[30].m_position = Vec3(-2.f, 0.5f, 0.f);
	m_localVerts[31].m_position = Vec3(-2.f, -0.5f, 0.f);
	m_localVerts[32].m_position = Vec3(-2.75f, 0.f, 0.f);

	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_uvTexCoords = Vec2(0.f, 0.f);
	}

	m_localVerts[30].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[31].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[32].m_color = Rgba8(255, 255, 0, 255);
}

EnemyBullet* EnemyShip::SpawnNewBullet()
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		if (m_game->m_enemyBullets[bulletIndex] == nullptr)
		{
			m_game->m_enemyBullets[bulletIndex] = new EnemyBullet(m_game, (GetForwardNormal() * 2.f) + Vec2(m_position.x, m_position.y), GetForwardNormal() * BULLET_SPEED);
			m_game->m_enemyBullets[bulletIndex]->m_orientationDegrees = m_orientationDegrees;
			return m_game->m_enemyBullets[bulletIndex];
		}
	}

	ERROR_RECOVERABLE("Bullets array is full");
	return nullptr;
}