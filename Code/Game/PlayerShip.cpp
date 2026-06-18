#include "Game/PlayerShip.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Bullet.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Audio/AudioSystem.hpp"

PlayerShip::PlayerShip(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity )
	: Entity(Owner, startingPosition, startingVelocity, 1)
{
InitializeLocalVerts();
m_orientationDegrees = 0.f;
m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
m_shieldCooldown = PLAYER_SHIP_SHIELD_COOLDOWN;
m_shieldHealth = 1.f;
m_game = Owner;
}

void PlayerShip::Update()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	if (m_health <= 0 && !m_isInvulnerable)
	{
		setDead();
	}

	m_velocity += GetForwardNormal() * PLAYER_SHIP_ACCELERATION * m_thrustFraction * dt;

	InvulnerabilityHandling();
	DashHandling();
	ShieldHandling();

	m_position += ( m_velocity * dt);

	SlowIfShooting();

	if (m_thrustFraction == 0.0f)
	{
		m_velocity -= m_velocity * 2.f * dt;
	}

	m_thrustFraction = 0.f;

	UpdateFromKeyboard();
	UpdateFromController();

	BounceOffBase();
	BounceOffWalls();
}

void PlayerShip::Render() const
{
	if(m_isDead)
		return;

	Vertex tempShipWorldVerts[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
		if (m_isInvulnerable)
		{
			tempShipWorldVerts[vertIndex].m_color = Rgba8(0,0,255);
		}
	}

	if(m_thrustFraction > 0.f)
	{
		float thrustFireRandomizer = g_rng->RollRandomFloatInRange(-0.5f, 0.5f);
		tempShipWorldVerts[32].m_position = Vec3(-2.f + (m_thrustFraction * -5.f), thrustFireRandomizer, 0.f);
	}

	RenderShieldHandling();

	TransformVertexArrayXY3D(NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_engine->m_render->DrawVertexArray(NUM_SHIP_VERTS, tempShipWorldVerts);
}

void PlayerShip::setDead()
{
	if (!m_isDead)
	{
		g_engine->m_audio->StartSound(m_game->m_shipExlposion, false, 0.75f, 0.0f, 1.0f, false);
		g_engine->m_audio->StartSound(m_game->m_deathSound);

		m_game->AddScreenShake(5.f);

		m_isDead = true;
		m_game->m_lives	-= 1;

		int debrisCount = g_rng->RollRandomIntInRange(5,30);
		m_game->SpawnNewDebrisCluster(debrisCount, m_position, (m_velocity*0.3f), m_cosmeticRadius*0.8f, Rgba8(255, 255, 255, 255), 3.f, 4.f);
	}
}

void PlayerShip::InitializeLocalVerts()
{
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		m_localVerts[vertIndex].m_color = Rgba8(255, 255, 255, 255);
		m_localVerts[vertIndex].m_uvTexCoords = Vec2(0.f, 0.f);
	}

	//HULL
	m_localVerts[0].m_position = Vec3(3.f, 0.f, 0.f);
	m_localVerts[1].m_position = Vec3(0.f, -1.f, 0.f);
	m_localVerts[2].m_position = Vec3(0.f, 1.f, 0.f);

	m_localVerts[3].m_position = Vec3(-2.f, 0.f, 0.f);
	m_localVerts[4].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[5].m_position = Vec3(0.f, -1.f, 0.f);

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
	m_localVerts[14].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[13].m_position = Vec3(0.f, 2.f, 0.f);

	m_localVerts[15].m_position = Vec3(1.f, 1.5f, 0.f);
	m_localVerts[17].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[16].m_position = Vec3(0.f, 1.f, 0.f);

	m_localVerts[12].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[13].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[14].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[15].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[16].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[17].m_color = Rgba8(0, 0, 255, 255);

	//LEFT WING
	m_localVerts[18].m_position = Vec3(-2.f, 5.f, 0.f);
	m_localVerts[19].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[20].m_position = Vec3(-1.f, 1.5f, 0.f);

	//RIGHT BLUE THING LOL
	m_localVerts[21].m_position = Vec3(-1.f, -1.5f, 0.f);
	m_localVerts[23].m_position = Vec3(0.f, -2.f, 0.f);
	m_localVerts[22].m_position = Vec3(0.f, -1.f, 0.f);

	m_localVerts[24].m_position = Vec3(1.f, -1.5f, 0.f);
	m_localVerts[26].m_position = Vec3(0.f, -1.f, 0.f);
	m_localVerts[25].m_position = Vec3(0.f, -2.f, 0.f);

	m_localVerts[21].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[22].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[23].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[24].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[25].m_color = Rgba8(0, 0, 255, 255);
	m_localVerts[26].m_color = Rgba8(0, 0, 255, 255);

	//RIGHT WING
	m_localVerts[27].m_position = Vec3(-2.f, -5.f, 0.f);
	m_localVerts[28].m_position = Vec3(-1.f, -1.5f, 0.f);
	m_localVerts[29].m_position = Vec3(0.f, -2.f, 0.f);

	//Thrust Triangle
	m_localVerts[30].m_position = Vec3(-2.f, 0.5f, 0.f);
	m_localVerts[31].m_position = Vec3(-2.f, -0.5f, 0.f);
	m_localVerts[32].m_position = Vec3(-2.75f, 0.f, 0.f);

	m_localVerts[30].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[31].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[32].m_color = Rgba8(255, 255, 0, 255);
}


void PlayerShip::Respawn()
{
	if(m_isDead)
	{
		m_InvulnerableTimer = INVULNERABILITY_TIMER;
		m_isDead = false;
		m_health = 1;
		m_position = Vec2(100.f, WORLD_SIZE_Y - 100.f);
		m_velocity = Vec2(0.f, 0.f);
		m_orientationDegrees = 0.f;
	}
}


void PlayerShip::UpdateFromKeyboard()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	if(g_theApp->m_isPaused == false)
	{
		InputSystem* input = g_engine->m_input;

		if (input->IsKeyDown('E') && !m_isDead)
		{
			m_thrustFraction = 1.f;
		}

		if (input->IsKeyDown('S'))
		{
			m_orientationDegrees += PLAYER_SHIP_TURN_SPEED * dt;
		}

		if (input->IsKeyDown('F'))
		{
			m_orientationDegrees -= PLAYER_SHIP_TURN_SPEED * dt;
		}

		if (input->IsKeyDown('D'))
		{
			m_thrustFraction = -0.5f;
		}
		
		if (input->IsKeyDown(' ') && !m_isDead && !g_theApp->m_isPaused && m_game->m_gamestate != Game::GAMESTATE_ATTRACT)
		{
			m_isShooting = true;

			if (m_bulletTimer > 0)
			{
				m_bulletTimer -= dt;
			}
			else
			{
				g_engine->m_audio->StartSound(m_game->m_pewSound, false, 0.5f * (static_cast<float>(m_game->m_bulletDamage) / 2.f));
				SpawnNewBullet();
				m_bulletTimer = PLAYER_SHIP_FIRE_SPEED;
			}
		}
		else
		{
			m_isShooting = false;
		}

		if (input->WasKeyJustPressed('N') && m_game->m_lives >= 0)
		{
			Respawn();
		}

		if (input->WasKeyJustPressed('R') && m_game->m_lives >= 0)
		{
			if (m_dashCooldown <= 0.f)
			{
				m_dashTimer = 0.25f;
				m_dashCooldown = 2.0f;
			}
		}

		if (input->WasKeyJustPressed('C'))
		{ 
			m_hasWeaponUpgrade = true;
			m_hasShield = true;
			m_hasDash = true;
			m_shieldHealth = 1.f;
		}
	}
}


void PlayerShip::UpdateFromController()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	if (g_theApp->m_isPaused == false)
	{
		XboxController const& controller = g_engine->m_input->GetController(0); // #ToDo: support multiple players?

		// Respawn
		if (m_isDead)
		{
			if (controller.WasButtonJustPressed(XboxButtonID::B) && m_game->m_lives >= 0)
			{
				Respawn();
			}
			return;
		}

		// Drive
		float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
		if (leftStickMagnitude > 0.f && !m_isDead)
		{
			m_thrustFraction = leftStickMagnitude;
			m_orientationDegrees = controller.GetLeftStick().GetOrientationDegrees();
		}

		// Shoot
		if (controller.IsButtonDown(XboxButtonID::A) && !m_isDead && !g_theApp->m_isPaused && m_game->m_gamestate != Game::GAMESTATE_ATTRACT)
		{
			m_isShooting = true;

			if (m_bulletTimer > 0)
			{
				m_bulletTimer -= dt;
			}
			else
			{
				g_engine->m_audio->StartSound(m_game->m_pewSound, false, 0.5f * (static_cast<float>(m_game->m_bulletDamage) / 2.f));
				SpawnNewBullet();
				m_bulletTimer = PLAYER_SHIP_FIRE_SPEED;
			}
		}
		else
		{
			m_isShooting = false;
		}


		if (controller.WasButtonJustPressed(XboxButtonID::X) && !m_isDead && !g_theApp->m_isPaused && m_game->m_gamestate != Game::GAMESTATE_ATTRACT && m_hasDash)
		{
			if (m_dashCooldown <= 0.f)
			{
				m_dashTimer = 0.25f;
				m_dashCooldown = 2.0f;
			}
		}

		if (m_dashCooldown <= 0.f && m_dashTimer <= 0.f)
		{
			m_dashCooldown -= dt;
		}
	}
}


void PlayerShip::DashHandling()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	// If dash is active, apply dash velocity each frame and tick timer down
	if (m_dashTimer > 0.f)
	{
		m_dashTimer -= dt;

		const float DASH_SPEED = 300.f; 
		m_velocity = GetForwardNormal() * DASH_SPEED;
	}
	if (m_dashCooldown > 0.f)
	{
		m_dashCooldown -= dt;
		if (m_dashCooldown < 0.f)
			m_dashCooldown = 0.f;
	}

	if (m_dashTimer <= 0.f)
	{
		m_velocity.x = GetClamped(m_velocity.x, -50.f, 50.f);
		m_velocity.y = GetClamped(m_velocity.y, -50.f, 50.f);
	}
}


void PlayerShip::InvulnerabilityHandling()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	if (m_InvulnerableTimer > 0.0f)
	{
		m_health = 1;
		m_InvulnerableTimer -= dt;
		m_isInvulnerable = true;
	}
	else
	{
		m_isInvulnerable = false;
	}
}


void PlayerShip::ShieldHandling()
{
	float dt = (float)m_game->m_gameClock->GetDeltaSeconds();
	if (m_shieldCooldown > 0.f && m_shieldHealth == 0.f)
	{
		m_shieldCooldown -= dt;
	}
	else if (m_hasShield && m_shieldCooldown <= 0.f)
	{
		m_shieldHealth = 1.f;
		m_shieldCooldown = PLAYER_SHIP_SHIELD_COOLDOWN;
	}
}


void PlayerShip::SlowIfShooting()
{
	if (m_isShooting)
	{
		m_velocity.x = GetClamped(m_velocity.x, -5.f, 5.f);
		m_velocity.y = GetClamped(m_velocity.y, -5.f, 5.f);
	}
}


void PlayerShip::BounceOffWalls()
{
	if (m_position.x > WORLD_SIZE_X - m_cosmeticRadius)
	{
		m_position.x = WORLD_SIZE_X - m_cosmeticRadius;
		m_velocity.x = -m_velocity.x;
	}
	if (m_position.x < m_cosmeticRadius)
	{
		m_position.x = m_cosmeticRadius + 0.1f;
		m_velocity.x = -m_velocity.x;
	}
	if (m_position.y > WORLD_SIZE_Y - m_cosmeticRadius)
	{
		m_position.y = WORLD_SIZE_Y - m_cosmeticRadius;
		m_velocity.y = -m_velocity.y;
	}
	if (m_position.y < m_cosmeticRadius)
	{
		m_position.y = m_cosmeticRadius + 0.1f;
		m_velocity.y = -m_velocity.y;
	}
}


void PlayerShip::BounceOffBase()
{
	if (DoDiscsOverlap(Vec2(0.f, WORLD_SIZE_Y), BASE_PHYSICS_RADIUS + 2.5f, m_position, m_cosmeticRadius))
	{
		m_velocity = -m_velocity;
	}
}


void PlayerShip::RenderShieldHandling() const
{
	if (m_hasShield && m_shieldHealth > 0)
	{
		DebugDrawRing(m_position, PLAYER_SHIP_SHIELD_RADIUS, 1.f, Rgba8(0, 0, 255));
	}
}


Bullet* PlayerShip::SpawnNewBullet()
{
	float bullet_cone_orientation;
	m_game->AddScreenShake(0.3f);

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		if (m_game->m_bullets[bulletIndex] == nullptr)
		{
			m_game->m_bullets[bulletIndex] = new Bullet(m_game, (m_game->m_ship1->GetForwardNormal() * 2.f) + Vec2(m_game->m_ship1->m_position.x, m_game->m_ship1->m_position.y), m_game->m_ship1->GetForwardNormal() * BULLET_SPEED);
			if (m_hasWeaponUpgrade)
			{
				bullet_cone_orientation = g_rng->RollRandomFloatInRange(-2.f, 2.f);
			}
			else
			{
				bullet_cone_orientation = g_rng->RollRandomFloatInRange(-10.f, 10.f);
			}
			m_game->m_bullets[bulletIndex]->m_orientationDegrees = m_game->m_ship1->m_orientationDegrees + bullet_cone_orientation;
			return m_game->m_bullets[bulletIndex];
		}
	}

	ERROR_RECOVERABLE("Bullets array is full");
	return nullptr;
}

