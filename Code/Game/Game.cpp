#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Bullet.hpp"
#include "Game/EnemyBullet.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Wasp.hpp"
#include "Game/EnemyShip.hpp"
#include "Game/Debris.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"


Game::Game()
{
	m_worldCamera = new Camera();
	m_worldCamera->SetOrthographicView(Vec2(0, 0), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));

	m_screenCamera = new Camera();
	m_screenCamera->SetOrthographicView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));

	m_ship1 = new PlayerShip(this, Vec2(50.f, WORLD_SIZE_Y - 50.f), Vec2(0.f, 0.f));
	m_enemyShips[0] = new EnemyShip(this, Vec2(WORLD_SIZE_X, 0.f), Vec2(0.f, 0.f));
	m_baseHealth = MAX_BASE_HEALTH;

	CreateSound();
	GenerateStarfield();
	m_gameClock = new Clock(g_theApp->m_systemClock);
	PrintControlsToConsole();
}


Game::~Game()
{
	delete m_ship1;
	m_ship1 = nullptr;

	DeleteAllEntities();
}


void Game::Update()
{
	m_prevState = m_gamestate;
	UpdateFromKeyboard();
	UpdateFromController();

	if (m_gamestate == GAMESTATE_ATTRACT)
	{
		return;
	}

	LoseGameHandling();

	StartWaveOrShopHandling();

	DeleteGarbage();

	UpdateEntities();
	UpdateCameras();
}


void Game::Render() const
{
	//ATTRACT USES ITS OWN CAMERA
	if (m_gamestate == GAMESTATE_ATTRACT)
	{
		RenderAttractMode();
		return;
	}

	// World Camera
	g_engine->m_render->BeginCamera(*m_worldCamera);
	g_engine->m_render->DrawVertexArray(NUM_STAR_VERTS, m_starVerts);
	if (m_gamestate == GAMESTATE_SHOP)
	{
		RenderShop();
	}

	RenderEntities();
	if (g_debugDraw)
	{
		DebugRenderEntities();
	}

	RenderBase();
	g_engine->m_render->EndCamera(*m_worldCamera);

	// Screen Camera
	g_engine->m_render->BeginCamera(*m_screenCamera);
	RenderUI();
	
	if (m_wave >= NUM_WAVES)
	{
		RenderWinOverlay();
	}
	
	if (GetClock()->isPaused())
	{
		RenderPaused();
	}

	g_engine->m_render->EndCamera(*m_screenCamera);
}
 
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// UPDATE HELPERS
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Game::UpdateEntities() 
{
	//Update Ship  *Updates from Controller Input as well
	m_ship1->Update();

	//Update Bullets
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		if (m_bullets[bulletIndex] != nullptr && !(m_bullets[bulletIndex]->m_isDead))
		{
			m_bullets[bulletIndex]->Update();

			//Check for collision with any asteroids
			for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
			{
				if (m_asteroids[asteroidIndex] != nullptr)
				{
					if (DoDiscsOverlap(m_bullets[bulletIndex]->m_position, m_bullets[bulletIndex]->m_physicsRadius, m_asteroids[asteroidIndex]->m_position, m_asteroids[asteroidIndex]->m_physicsRadius))
					{
						m_bullets[bulletIndex]->TakeDamage(m_bulletDamage);
						m_asteroids[asteroidIndex]->TakeDamage(m_bulletDamage);
					}
				}
			}

			//Check for collision with any beetles
			for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
			{
				if (m_beetles[beetleIndex] != nullptr)
				{
					if (DoDiscsOverlap(m_bullets[bulletIndex]->m_position, m_bullets[bulletIndex]->m_physicsRadius, m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_physicsRadius))
					{
						m_bullets[bulletIndex]->TakeDamage(m_bulletDamage);
						m_beetles[beetleIndex]->TakeDamage(m_bulletDamage);
					}
				}
			}

			//Check for collision with any Wasps
			for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
			{
				if (m_wasps[waspIndex] != nullptr)
				{
					if (DoDiscsOverlap(m_bullets[bulletIndex]->m_position, m_bullets[bulletIndex]->m_physicsRadius, m_wasps[waspIndex]->m_position, m_wasps[waspIndex]->m_physicsRadius))
					{
						m_bullets[bulletIndex]->TakeDamage(m_bulletDamage);
						m_wasps[waspIndex]->TakeDamage(m_bulletDamage);
					}
				}
			}

			//Check for collision with any Enemy Ships
			for (int shipIndex = 0; shipIndex < MAX_ENEMY_SHIPS; ++shipIndex)
			{
				if (m_enemyShips[shipIndex] != nullptr)
				{
					if (DoDiscsOverlap(m_bullets[bulletIndex]->m_position, m_bullets[bulletIndex]->m_physicsRadius, m_enemyShips[shipIndex]->m_position, m_enemyShips[shipIndex]->m_physicsRadius))
					{
						m_bullets[bulletIndex]->TakeDamage(m_bulletDamage);
						m_enemyShips[shipIndex]->TakeDamage(m_bulletDamage);
					}
				}
			}

			if (m_bullets[bulletIndex]->m_health <= 0)
			{
				m_bullets[bulletIndex]->setDead();
			}

		}
	}

	//Update Enemy Bullets
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex)
	{
		EnemyBullet* bullet = m_enemyBullets[bulletIndex];
		if (bullet != nullptr && !(bullet->m_isDead))
		{
			m_enemyBullets[bulletIndex]->Update();

			//Check for collision with any asteroids
			for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
			{

				if (m_asteroids[asteroidIndex] != nullptr)
				{
					bool doesOverlap = DoDiscsOverlap(
						m_enemyBullets[bulletIndex]->m_position, 
						m_enemyBullets[bulletIndex]->m_physicsRadius, 
						m_asteroids[asteroidIndex]->m_position, 
						m_asteroids[asteroidIndex]->m_physicsRadius
					);

					if ( doesOverlap )
					{
						m_enemyBullets[bulletIndex]->TakeDamage(m_bulletDamage);
						m_asteroids[asteroidIndex]->TakeDamage(m_bulletDamage);
					}
				}
			}

			//Check for collision with Player Ship
			if (DoDiscsOverlap(m_ship1->m_position, m_ship1->m_physicsRadius, m_enemyBullets[bulletIndex]->m_position, m_enemyBullets[bulletIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				m_ship1->m_health -= 1;
				m_enemyBullets[bulletIndex]->m_health -= 1;
			}
			
			//Check for collision with Shield Radius
			if (DoDiscsOverlap(m_ship1->m_position, PLAYER_SHIP_SHIELD_RADIUS, m_enemyBullets[bulletIndex]->m_position, m_enemyBullets[bulletIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				//Does player have shield Up?
				if(m_ship1->m_hasShield && m_ship1->m_shieldHealth > 0)
				{ 
					m_enemyBullets[bulletIndex]->m_health -= 10;
					m_ship1->m_shieldHealth = 0.f;
				}
			}

			if (m_enemyBullets[bulletIndex]->m_health <= 0)
			{
				m_enemyBullets[bulletIndex]->setDead();
			}

		}
	}

	//update Asteroids
	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		if (m_asteroids[asteroidIndex] != nullptr && !(m_asteroids[asteroidIndex]->m_isDead))
		{
			m_asteroids[asteroidIndex]->Update();

			//Ship and Asteroid Collision
			if (DoDiscsOverlap(m_ship1->m_position, m_ship1->m_physicsRadius, m_asteroids[asteroidIndex]->m_position, m_asteroids[asteroidIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				m_ship1->m_health -= 1;
				m_asteroids[asteroidIndex]->m_health -= 1;
			}

			//Check for collision with Shield Radius
			if (DoDiscsOverlap(m_ship1->m_position, PLAYER_SHIP_SHIELD_RADIUS, m_asteroids[asteroidIndex]->m_position, m_asteroids[asteroidIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				//Does player have shield Up?
				if (m_ship1->m_hasShield && m_ship1->m_shieldHealth > 0)
				{
					m_asteroids[asteroidIndex]->m_health -= 10;
					m_ship1->m_shieldHealth = 0.f;
				}
			}

			if (m_asteroids[asteroidIndex]->m_health <= 0)
			{
				g_engine->m_audio->StartSound(m_explosion, false, 0.75f, 0.0f, 1.0f, false);
				m_asteroids[asteroidIndex]->setDead();
			}
		}
	}

	//Update Beetles
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		if (m_beetles[beetleIndex] != nullptr && !(m_beetles[beetleIndex]->m_isDead))
		{
			//Update Beetle Orientation and Velocity
			m_beetles[beetleIndex]->Update();

			//Ship and Beetle Collision
			if (DoDiscsOverlap(m_ship1->m_position, m_ship1->m_physicsRadius, m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				m_ship1->m_health -= 1;
				m_beetles[beetleIndex]->m_health -= 1;
			}

			//Base and Beetle Collision
			if (DoDiscsOverlap(Vec2(0.f,WORLD_SIZE_Y), BASE_PHYSICS_RADIUS, m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_physicsRadius))
			{
				m_baseHealth -= 1.f;
				m_beetles[beetleIndex]->m_health -= 10;
			}

			//Beetle to Beetle Collision
			for (int beetleIndex_two = 0; beetleIndex_two < MAX_BEETLES; ++beetleIndex_two)
			{
				if (m_beetles[beetleIndex_two] != nullptr)
				{
					if (DoDiscsOverlap(m_beetles[beetleIndex_two]->m_position, m_beetles[beetleIndex_two]->m_physicsRadius, m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_physicsRadius) && m_beetles[beetleIndex_two] != m_beetles[beetleIndex])
					{
						PushDiscsOutOfEachOther2D(m_beetles[beetleIndex_two]->m_position, m_beetles[beetleIndex_two]->m_physicsRadius, m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_physicsRadius);
					}
				}
			}

			//Check for collision with Shield Radius
			if (DoDiscsOverlap(m_ship1->m_position, PLAYER_SHIP_SHIELD_RADIUS, m_beetles[beetleIndex]->m_position, m_beetles[beetleIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				//Does player have shield Up?
				if (m_ship1->m_hasShield && m_ship1->m_shieldHealth > 0)
				{
					m_beetles[beetleIndex]->m_health -= 10;
					m_ship1->m_shieldHealth = 0.f;
				}
			}

			if (m_beetles[beetleIndex]->m_health <= 0)
			{
				g_engine->m_audio->StartSound(m_explosion, false, 0.75f, 0.0f, 1.0f, false);
				m_beetles[beetleIndex]->setDead();
			}
		}
	}

	//Update Wasps
	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		if (m_wasps[waspIndex] != nullptr && !(m_wasps[waspIndex]->m_isDead))
		{
			//Update Wasp Orientation and Velocity
			m_wasps[waspIndex]->m_orientationDegrees = Atan2Degrees((m_ship1->m_position.y - m_wasps[waspIndex]->m_position.y), (m_ship1->m_position.x - m_wasps[waspIndex]->m_position.x));
			m_wasps[waspIndex]->Update();

			//Ship and Wasp Collision
			if (DoDiscsOverlap(m_ship1->m_position, m_ship1->m_physicsRadius, m_wasps[waspIndex]->m_position, m_wasps[waspIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				m_ship1->m_health -= 1;
				m_wasps[waspIndex]->m_health -= 1;
			}

			//Check for collision with Shield Radius
			if (DoDiscsOverlap(m_ship1->m_position, PLAYER_SHIP_SHIELD_RADIUS, m_wasps[waspIndex]->m_position, m_wasps[waspIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				//Does player have shield Up?
				if (m_ship1->m_hasShield && m_ship1->m_shieldHealth > 0)
				{
					m_wasps[waspIndex]->m_health -= 10;
					m_ship1->m_shieldHealth = 0.f;
				}
			}

			if (m_wasps[waspIndex]->m_health <= 0)
			{
				g_engine->m_audio->StartSound(m_explosion, false, 0.75f, 0.0f, 1.0f, false);
				m_wasps[waspIndex]->setDead();
			}
		}
	}

	//Update Enemy Ships
	for (int shipIndex = 0; shipIndex < MAX_ENEMY_SHIPS; shipIndex++)
	{
		if (m_enemyShips[shipIndex] != nullptr && !(m_enemyShips[shipIndex]->m_isDead))
		{
			m_enemyShips[shipIndex]->Update();

			//Player Ship and Enemy Ship Collision
			if (DoDiscsOverlap(m_ship1->m_position, m_ship1->m_physicsRadius, m_enemyShips[shipIndex]->m_position, m_enemyShips[shipIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				m_ship1->m_health -= 1;
				m_enemyShips[shipIndex]->m_health -= 1;
			}

			//Beetle to Beetle Collision
			for (int shipIndex_two = 0; shipIndex_two < MAX_ENEMY_SHIPS; ++shipIndex_two)
			{
				if (m_enemyShips[shipIndex_two] != nullptr)
				{
					if (DoDiscsOverlap(m_enemyShips[shipIndex_two]->m_position, m_enemyShips[shipIndex_two]->m_physicsRadius, m_enemyShips[shipIndex]->m_position, m_enemyShips[shipIndex]->m_physicsRadius) && m_enemyShips[shipIndex_two] != m_enemyShips[shipIndex])
					{
						PushDiscsOutOfEachOther2D(m_enemyShips[shipIndex_two]->m_position, m_enemyShips[shipIndex_two]->m_physicsRadius, m_enemyShips[shipIndex]->m_position, m_enemyShips[shipIndex]->m_physicsRadius);
					}
				}
			}

			//Check for collision with Shield Radius
			if (DoDiscsOverlap(m_ship1->m_position, PLAYER_SHIP_SHIELD_RADIUS, m_enemyShips[shipIndex]->m_position, m_enemyShips[shipIndex]->m_physicsRadius) && (!m_ship1->m_isDead))
			{
				//Does player have shield Up?
				if (m_ship1->m_hasShield && m_ship1->m_shieldHealth > 0)
				{
					m_enemyShips[shipIndex]->m_health -= 10;
					m_ship1->m_shieldHealth = 0.f;
				}
			}

			if (m_enemyShips[shipIndex]->m_health <= 0)
			{
				g_engine->m_audio->StartSound(m_explosion, false, 0.75f, 0.0f, 1.0f, false);
				m_enemyShips[shipIndex]->setDead();
			}
		}
	}

	//Update Debris
	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		if (m_debris[debrisIndex] != nullptr && !(m_debris[debrisIndex]->m_isDead))
		{
			m_debris[debrisIndex]->Update();
		}
	}
}

void Game::UpdateShop()
{
	if (DoDiscsOverlap(m_ship1->m_position, m_ship1->m_physicsRadius, Vec2(100.f, 170.f), SHOP_ITEM_RADIUS) && (!m_ship1->m_isDead) && !(m_ship1->m_hasWeaponUpgrade) && !(m_ship1->m_isInvulnerable))
	{
		m_ship1->m_hasWeaponUpgrade = true;
		m_bulletDamage = 2;
		m_gamestate = GAMESTATE_PLAYING;
	}

	if (DoDiscsOverlap(m_ship1->m_position, m_ship1->m_physicsRadius, Vec2(200.f, 170.f), SHOP_ITEM_RADIUS) && (!m_ship1->m_isDead) && !(m_ship1->m_hasDash) && !(m_ship1->m_isInvulnerable))
	{
		m_ship1->m_hasDash = true;
		m_gamestate = GAMESTATE_PLAYING;
	}

	if (DoDiscsOverlap(m_ship1->m_position, m_ship1->m_physicsRadius, Vec2(300.f, 170.f), SHOP_ITEM_RADIUS) && (!m_ship1->m_isDead) && !(m_ship1->m_hasShield) && !(m_ship1->m_isInvulnerable))
	{
		m_ship1->m_hasShield = true;
		m_ship1->m_shieldHealth = 1.f;
		m_gamestate = GAMESTATE_PLAYING;
	}
}


void Game::UpdateCameras()
{
	float dt = (float)m_gameClock->GetDeltaSeconds();
	Vec2 viewMins(m_ship1->m_position.x - SCREEN_SIZE_X * 0.5f, m_ship1->m_position.y - SCREEN_SIZE_Y * 0.5f);
	Vec2 viewMaxs(m_ship1->m_position.x + SCREEN_SIZE_X * 0.5f, m_ship1->m_position.y + SCREEN_SIZE_Y * 0.5f);

	if (viewMins.x < 0.f)
	{
		viewMins.x = 0.f;
		viewMaxs.x = SCREEN_SIZE_X;
	}
	if (viewMaxs.x > WORLD_SIZE_X)
	{
		viewMaxs.x = WORLD_SIZE_X;
		viewMins.x = WORLD_SIZE_X - SCREEN_SIZE_X;
	}

	if (viewMins.y < 0.f)
	{
		viewMins.y = 0.f;
		viewMaxs.y = SCREEN_SIZE_Y;
	}
	if (viewMaxs.y > WORLD_SIZE_Y)
	{
		viewMaxs.y = WORLD_SIZE_Y;
		viewMins.y = WORLD_SIZE_Y - SCREEN_SIZE_Y;
	}

	m_screenCamera->SetOrthographicView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	m_worldCamera->SetOrthographicView(viewMins, viewMaxs);

	float shakeX = g_rng->RollRandomFloatInRange(-m_screenShakeAmount, m_screenShakeAmount);
	float shakeY = g_rng->RollRandomFloatInRange(-m_screenShakeAmount, m_screenShakeAmount);
	Vec2 shakeOffset(shakeX, shakeY);

	m_screenCamera->SetOrthographicView(Vec2(0.f, 0.f), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	m_worldCamera->SetOrthographicView(viewMins + shakeOffset, viewMaxs + shakeOffset);

	constexpr float MAX_SCREEN_SHAKE = 5.f;
	constexpr float SCREEN_SHAKE_REDUCTION_PER_SEC = 5.f;

	m_screenShakeAmount -= SCREEN_SHAKE_REDUCTION_PER_SEC * dt;
	m_screenShakeAmount = GetClamped(m_screenShakeAmount, 0.f, MAX_SCREEN_SHAKE);
}

void Game::AddScreenShake(float additionalShake)
{
	m_screenShakeAmount += additionalShake;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// RENDERING HELPERS
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Game::RenderAttractMode() const
{
	Camera* attractCamera = new Camera();
	attractCamera->SetOrthographicView(Vec2(0, 0), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));

	static float attractModeTimer = 0.f;
	attractModeTimer += 1.f / 145.f;
	float flashSpeed = 1.0f;                                
	float flash = (SinDegrees(attractModeTimer * flashSpeed * 360.f) * 0.5f + 0.5f); 
	unsigned char alpha = static_cast<unsigned char>(flash * 255.f);

	Vertex playButtonVerts[3];
	playButtonVerts[0].m_position = Vec3(-5.f, -5.f, 0.f);
	playButtonVerts[1].m_position = Vec3(-5.f, 5.f, 0.f);
	playButtonVerts[2].m_position = Vec3(5.f, 0.f, 0.f);
	for (int vertIndex = 0; vertIndex < 3; ++vertIndex)
	{
		playButtonVerts[vertIndex].m_color = Rgba8(0, 255, 0, alpha);
	}

	Vertex playButtonVertsCopy[3];
	for (int i = 0; i < 3; i++)
	{
		playButtonVertsCopy[i] = playButtonVerts[i];
	}
	TransformVertexArrayXY3D(3, playButtonVerts, 1.f, 0.0, Vec2(SCREEN_SIZE_X + 50.f, SCREEN_SIZE_Y - 50.f));
	TransformVertexArrayXY3D(3, playButtonVertsCopy, 1.f, 180.0, Vec2(SCREEN_SIZE_X - 50.f, SCREEN_SIZE_Y - 50.f));

	//Player Ship Verts
	Vertex tempShipWorldVerts[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		tempShipWorldVerts[vertIndex] = m_ship1->m_localVerts[vertIndex];
	}
	float thrustFireRandomizer = g_rng->RollRandomFloatInRange(-0.5f, 0.5f);
	tempShipWorldVerts[32].m_position = Vec3(-2.f + (1.f * -5.f), thrustFireRandomizer, 0.f);
	TransformVertexArrayXY3D(NUM_SHIP_VERTS, tempShipWorldVerts, 10.f, 90.f, Vec2((SCREEN_SIZE_X - (SCREEN_SIZE_X/2)), SCREEN_SIZE_Y));

	//Enemy Ship Verts
	Vertex tempShipWorldVerts2[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
	{
		tempShipWorldVerts2[vertIndex] = m_ship1->m_localVerts[vertIndex];
	}
	thrustFireRandomizer = g_rng->RollRandomFloatInRange(-0.5f, 0.5f);
	tempShipWorldVerts2[32].m_position = Vec3(-2.f + (1.f * -5.f), thrustFireRandomizer, 0.f);
	TransformVertexArrayXY3D(NUM_SHIP_VERTS, tempShipWorldVerts2, 10.f, 90.f, Vec2(SCREEN_SIZE_X + (SCREEN_SIZE_X/2), SCREEN_SIZE_Y));

	std::vector<Vertex> playButtonTextVerts;
	std::vector<Vertex> titleTextVerts;
	std::vector<Vertex> titleShadowVerts;

	AddVertsForTextTriangles2D(playButtonTextVerts, "Press Start", Vec2(SCREEN_SIZE_X - 36.f, SCREEN_SIZE_Y - 57.f), 10.f, Rgba8(255, 255, 255));
	AddVertsForTextTriangles2D(playButtonTextVerts, "Defend the Earth from Beetles!", Vec2(SCREEN_SIZE_X - 75.f, SCREEN_SIZE_Y + 75.f), 7.f, Rgba8(255, 255, 255));
	AddVertsForTextTriangles2D(playButtonTextVerts, "If the Earth's Health drops to Zero or you Run out of lives it's game over!", Vec2(SCREEN_SIZE_X - 125.f, SCREEN_SIZE_Y + 65.f), 5.f, Rgba8(255, 255, 255));

	AddVertsForTextTriangles2D(titleTextVerts, "STARSHIP: GOLD", Vec2((SCREEN_SIZE_X/2) + 27.f, 30.f), 20.f, Rgba8(50, 50, 255));
	AddVertsForTextTriangles2D(titleTextVerts, "STARSHIP: GOLD", Vec2((SCREEN_SIZE_X/2) + 27.f, (SCREEN_SIZE_Y*2) - 30.f), 20.f, Rgba8(50, 50, 255));
	AddVertsForTextTriangles2D(titleShadowVerts, "STARSHIP: GOLD", Vec2((SCREEN_SIZE_X/2) + 25.f, 30.f), 20.f, Rgba8(255, 255, 255));
	AddVertsForTextTriangles2D(titleShadowVerts, "STARSHIP: GOLD", Vec2((SCREEN_SIZE_X/2) + 25.f, (SCREEN_SIZE_Y*2) - 30.f), 20.f, Rgba8(255, 255, 255));

	g_engine->m_render->BeginCamera(*m_screenCamera);
	g_engine->m_render->DrawVertexArray(NUM_STAR_VERTS, m_starVerts);
	g_engine->m_render->DrawVertexArray(3, playButtonVerts);
	g_engine->m_render->DrawVertexArray(3, playButtonVertsCopy);
	g_engine->m_render->DrawVertexArray(static_cast<int>(playButtonTextVerts.size()), playButtonTextVerts.data());
	g_engine->m_render->DrawVertexArray(NUM_SHIP_VERTS, tempShipWorldVerts2);
	g_engine->m_render->DrawVertexArray(NUM_SHIP_VERTS, tempShipWorldVerts);
	g_engine->m_render->DrawVertexArray(static_cast<int>(titleTextVerts.size()), titleTextVerts.data());
	g_engine->m_render->DrawVertexArray( static_cast<int>(titleShadowVerts.size()), titleShadowVerts.data() );
	g_engine->m_render->EndCamera(*m_screenCamera);
}


void Game::RenderEntities() const
{
	m_ship1->Render();

	//Render Bullets
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		if (m_bullets[bulletIndex] != nullptr)
		{
			m_bullets[bulletIndex]->Render();
		}
	}

	//Render Enemy Bullets
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		if (m_enemyBullets[bulletIndex] != nullptr)
		{
			m_enemyBullets[bulletIndex]->Render();
		}
	}

	//Render Asteroids
	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		if (m_asteroids[asteroidIndex] != nullptr)
		{
			m_asteroids[asteroidIndex]->Render();
		}
	}

	//Render Beetles
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		if (m_beetles[beetleIndex] != nullptr)
		{
			m_beetles[beetleIndex]->Render();
		}
	}

	//Render Wasps
	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		if (m_wasps[waspIndex] != nullptr)
		{
			m_wasps[waspIndex]->Render();
		}
	}

	//Render Enemy Ships
	for (int shipIndex = 0; shipIndex < MAX_ENEMY_SHIPS; shipIndex++)
	{
		if (m_enemyShips[shipIndex] != nullptr)
		{
			m_enemyShips[shipIndex]->Render();
		}
	}

	//Render Debris
	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		if (m_debris[debrisIndex] != nullptr)
		{
			m_debris[debrisIndex]->Render();
		}
	}
}


void Game::DebugRenderEntities() const
{
	//Render Ship Debug
	m_ship1->RenderDebug();

	//Render Bullets Debug
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		if (m_bullets[bulletIndex] != nullptr)
		{
			m_bullets[bulletIndex]->RenderDebug();

			DebugDrawLine(m_bullets[bulletIndex]->m_position, m_ship1->m_position, 0.2f, Rgba8(50, 50, 50));
		}
	}

	//Render Enemy Bullets Debug
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		if (m_enemyBullets[bulletIndex] != nullptr)
		{
			m_enemyBullets[bulletIndex]->RenderDebug();

			DebugDrawLine(m_enemyBullets[bulletIndex]->m_position, m_ship1->m_position, 0.2f, Rgba8(50, 50, 50));
		}
	}

	//Render Asteroids Debug
	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		if (m_asteroids[asteroidIndex] != nullptr)
		{
			m_asteroids[asteroidIndex]->RenderDebug();
			
			DebugDrawLine(m_asteroids[asteroidIndex]->m_position, m_ship1->m_position, 0.2f, Rgba8(50, 50, 50));
		}
	}

	//Render Beetles Debug
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		if (m_beetles[beetleIndex] != nullptr)
		{

			DebugDrawLine(m_beetles[beetleIndex]->m_position, m_ship1->m_position, 0.2f, Rgba8(50, 50, 50));

			m_beetles[beetleIndex]->RenderDebug();

		}
	}

	//Render Wasps Debug
	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		if (m_wasps[waspIndex] != nullptr)
		{
			DebugDrawLine(m_wasps[waspIndex]->m_position, m_ship1->m_position, 0.2f, Rgba8(50, 50, 50));

			m_wasps[waspIndex]->RenderDebug();

		}
	}

	//Render Enemy Ships Debug
	for (int shipIndex = 0; shipIndex < MAX_ENEMY_SHIPS; shipIndex++)
	{
		if (m_enemyShips[shipIndex] != nullptr)
		{
			DebugDrawLine(m_enemyShips[shipIndex]->m_position, m_ship1->m_position, 0.2f, Rgba8(50, 50, 50));

			m_enemyShips[shipIndex]->RenderDebug();

		}
	}

	//Render Debris Debug
	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		if (m_debris[debrisIndex] != nullptr)
		{
			DebugDrawLine(m_debris[debrisIndex]->m_position, m_ship1->m_position, 0.2f, Rgba8(50, 50, 50));

			m_debris[debrisIndex]->RenderDebug();

		}
	}
}


void Game::RenderBase() const
{
	//SICK
	DebugDrawRing(Vec2(0.f, WORLD_SIZE_Y), BASE_PHYSICS_RADIUS, 8.f, Rgba8(0, 130, 255));
	DebugDrawRing(Vec2(0.f, WORLD_SIZE_Y), BASE_PHYSICS_RADIUS - 46.f, 88.f, Rgba8(0, 130, 255));

	Vertex landVerts[15];

	landVerts[0].m_position = Vec3(10.f, WORLD_SIZE_Y, 0.f);
	landVerts[1].m_position = Vec3(26.f, WORLD_SIZE_Y, 0.f);
	landVerts[2].m_position = Vec3(10.f, WORLD_SIZE_Y - 30.f, 0.f);

	landVerts[3].m_position = Vec3(15.f, WORLD_SIZE_Y, 0.f);
	landVerts[4].m_position = Vec3(35.f, WORLD_SIZE_Y, 0.f);
	landVerts[5].m_position = Vec3(21.f, WORLD_SIZE_Y - 10.f, 0.f);

	landVerts[6].m_position = Vec3(10.f, WORLD_SIZE_Y, 0.f);
	landVerts[7].m_position = Vec3(5.f, WORLD_SIZE_Y, 0.f);
	landVerts[8].m_position = Vec3(10.f, WORLD_SIZE_Y - 10.f, 0.f);

	landVerts[9].m_position = Vec3(20.f, WORLD_SIZE_Y - 30.f, 0.f);
	landVerts[10].m_position = Vec3(32.f, WORLD_SIZE_Y - 38.f, 0.f);
	landVerts[11].m_position = Vec3(40.f, WORLD_SIZE_Y - 30.f, 0.f);

	landVerts[12].m_position = Vec3(20.f, WORLD_SIZE_Y - 30.f, 0.f);
	landVerts[13].m_position = Vec3(35.f, WORLD_SIZE_Y - 25.f, 0.f);
	landVerts[14].m_position = Vec3(40.f, WORLD_SIZE_Y - 30.f, 0.f);

	for (int vertIndex = 3; vertIndex < 9; ++vertIndex)
	{
		landVerts[vertIndex].m_color = Rgba8(0, 153, 0);
	}
	for (int vertIndex = 0; vertIndex < 3; ++vertIndex)
	{
		landVerts[vertIndex].m_color = Rgba8(0, 125, 0);
	}
	for (int vertIndex = 9; vertIndex < 12; ++vertIndex)
	{
		landVerts[vertIndex].m_color = Rgba8(0, 125, 0);
	}
	for (int vertIndex = 12; vertIndex < 15; ++vertIndex)
	{
		landVerts[vertIndex].m_color = Rgba8(0, 153, 0);
	}

	g_engine->m_render->DrawVertexArray(15, landVerts);
}

void Game::RenderUI() const 
{

	//OUTPUT LIVES
	std::vector<Vertex> liveVerts;
	AddVertsForTextTriangles2D(liveVerts, "Lives:", Vec2(5.f, WORLD_SIZE_Y - 10.f), 5.f, Rgba8(255, 255, 255));
	g_engine->m_render->DrawVertexArray(static_cast<int>(liveVerts.size()), liveVerts.data());

	Vertex fakePlayerShipVerts[NUM_SHIP_VERTS];

	for (int livesIndex = 0; livesIndex < m_lives; ++livesIndex)
	{
		for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex)
		{
			fakePlayerShipVerts[vertIndex] = m_ship1->m_localVerts[vertIndex];
		}

		TransformVertexArrayXY3D(NUM_SHIP_VERTS, fakePlayerShipVerts, 1.f, 90.0, Vec2((32.f + (15.f * livesIndex)), WORLD_SIZE_Y - 8.f));
		g_engine->m_render->DrawVertexArray(NUM_SHIP_VERTS, fakePlayerShipVerts);
	}

	//OUTPUT BASE HEALTH
	std::vector<Vertex> baseHealthVerts;
	AddVertsForTextTriangles2D(baseHealthVerts, "EARTH HEALTH:", Vec2((SCREEN_SIZE_X / 2) + 25.f, WORLD_SIZE_Y - 10.f), 5.f, Rgba8(255, 255, 255));
	g_engine->m_render->DrawVertexArray(static_cast<int>(baseHealthVerts.size()), baseHealthVerts.data());
	DebugDrawLine(Vec2((SCREEN_SIZE_X / 2) + 70.f, WORLD_SIZE_Y - 7.f), Vec2((SCREEN_SIZE_X / 2) + 70.f + (m_baseHealth*10.f), WORLD_SIZE_Y - 7.f), 5.f, Rgba8(0,255,0));
	DebugDrawLine(Vec2((SCREEN_SIZE_X / 2) + 70.f + (MAX_BASE_HEALTH*10.f), WORLD_SIZE_Y - 7.f), Vec2((SCREEN_SIZE_X / 2) + 70.f + ((MAX_BASE_HEALTH - (10.f-m_baseHealth)) * 10.f), WORLD_SIZE_Y - 7.f), 5.f, Rgba8(255,0,0));

	//OUTPUT WARNING IF BEETLES CLOSE TO BASE
	if(BeetlesCloseToBase())
	{ 
		static float attractModeTimer = 0.f;
		attractModeTimer += 1.f / 145.f;
		float flashSpeed = 1.0f;
		float flash = (SinDegrees(attractModeTimer * flashSpeed * 360.f) * 0.5f + 0.5f);
		unsigned char alpha = static_cast<unsigned char>(flash * 255.f);

		std::vector<Vertex> beetleWarningVerts;
		AddVertsForTextTriangles2D(baseHealthVerts, "BEETLES APPROACHING EARTH!", Vec2((SCREEN_SIZE_X / 2) + 200.f, WORLD_SIZE_Y - 10.f), 5.f, Rgba8(255, 0, 0, alpha));
		g_engine->m_render->DrawVertexArray(static_cast<int>(baseHealthVerts.size()), baseHealthVerts.data());
	}

	//SHOW SHOP HEADER DURING SHOP PHASE
	if(m_gamestate == GAMESTATE_SHOP)
	{ 
		std::vector<Vertex> shopHeaderVerts;
		AddVertsForTextTriangles2D(shopHeaderVerts, "CHOOSE AN UPGRADE!", Vec2((SCREEN_SIZE_X / 2) + 50.f, WORLD_SIZE_Y - 30.f), 10.f, Rgba8(255, 0, 0));
		g_engine->m_render->DrawVertexArray(static_cast<int>(shopHeaderVerts.size()), shopHeaderVerts.data());
	}

	//RESPAWN NOTICE
	if (m_ship1->m_isDead && !g_theApp->m_isPaused && m_lives >= 0)
	{
		std::vector<Vertex> pauseTextVerts;
		AddVertsForTextTriangles2D(pauseTextVerts, "PRESS B TO RESPAWN", Vec2(WORLD_CENTER_X - 50.f, WORLD_CENTER_Y + 10.f), 10.f, Rgba8(255, 255, 255));
		g_engine->m_render->DrawVertexArray(static_cast<int>(pauseTextVerts.size()), pauseTextVerts.data());
	}

	//OUTPUT WAVE NUMBER
	std::vector<Vertex> waveVerts;
	std::string waveText = Stringf( "Wave: %i / %i", m_wave + 1, NUM_WAVES);
	AddVertsForTextTriangles2D(waveVerts, waveText, Vec2(WORLD_SIZE_X - 50.f, WORLD_SIZE_Y - 10.f), 5.f, Rgba8(255, 255, 255));
	g_engine->m_render->DrawVertexArray(static_cast<int>(waveVerts.size()), waveVerts.data());
}


void Game::RenderShop() const
{
	if(!(m_ship1->m_hasWeaponUpgrade))
	{ 
		DebugDrawRing(Vec2(100.f, 170.f), SHOP_ITEM_RADIUS, 2.f, Rgba8(255,0,0));

		std::vector<Vertex> weaponShopVerts;
		AddVertsForTextTriangles2D(weaponShopVerts, "WEAPON UPGRADE", Vec2(70.f, 190.f), SHOP_TEXT_SIZE, Rgba8(255, 0, 0));
		g_engine->m_render->DrawVertexArray(static_cast<int>(weaponShopVerts.size()), weaponShopVerts.data());

		std::vector<Vertex> weaponShopVerts2;
		AddVertsForTextTriangles2D(weaponShopVerts2, "BULLETS DO DOUBLE DAMAGE AND FIRING SPREAD DECREASED", Vec2(70.f, 185.f), SHOP_TEXT_SIZE - 5.f, Rgba8(255, 0, 0));
		g_engine->m_render->DrawVertexArray(static_cast<int>(weaponShopVerts2.size()), weaponShopVerts2.data());
	}

	if(!(m_ship1->m_hasDash))
	{ 
		DebugDrawRing(Vec2(200.f, 170.f), SHOP_ITEM_RADIUS, 2.f, Rgba8(0, 255, 0));

		std::vector<Vertex> dashShopVerts;
		AddVertsForTextTriangles2D(dashShopVerts, "DASH ABILITY", Vec2(175.f, 190.f), SHOP_TEXT_SIZE, Rgba8(0, 255, 0));
		g_engine->m_render->DrawVertexArray(static_cast<int>(dashShopVerts.size()), dashShopVerts.data());

		std::vector<Vertex> dashShopVerts2;
		AddVertsForTextTriangles2D(dashShopVerts2, "DASH USING X BUTTON OR SHIFT KEY( 2s cooldown)", Vec2(170.f, 185.f), SHOP_TEXT_SIZE -5.f, Rgba8(0, 255, 0));
		g_engine->m_render->DrawVertexArray(static_cast<int>(dashShopVerts2.size()), dashShopVerts2.data());
	}	

	if(!(m_ship1->m_hasShield))
	{ 
		DebugDrawRing(Vec2(300.f, 170.f), SHOP_ITEM_RADIUS, 2.f, Rgba8(0, 0, 255));

		std::vector<Vertex> shieldShopVerts;
		AddVertsForTextTriangles2D(shieldShopVerts, "SHIELD ABILITY", Vec2(270.f, 190.f), SHOP_TEXT_SIZE, Rgba8(0, 0, 255));
		g_engine->m_render->DrawVertexArray(static_cast<int>(shieldShopVerts.size()), shieldShopVerts.data());

		std::vector<Vertex> shieldShopVerts2;
		AddVertsForTextTriangles2D(shieldShopVerts2, "GAIN A SHIELD THAT DESTROYS ENTITIES AND DISAPPEARS(6s cooldown)", Vec2(270.f, 185.f), SHOP_TEXT_SIZE -5.f, Rgba8(0, 0, 255));
		g_engine->m_render->DrawVertexArray(static_cast<int>(shieldShopVerts2.size()), shieldShopVerts2.data());
	}
}

void Game::RenderPaused() const
{
	Vertex pauseVerts[6];

	pauseVerts[0].m_position = Vec3( WORLD_SIZE_X, WORLD_SIZE_Y, 0.f);
	pauseVerts[1].m_position = Vec3( WORLD_SIZE_X, 0.f, 0.f);
	pauseVerts[2].m_position = Vec3( 0.f, WORLD_SIZE_Y, 0.f);

	pauseVerts[3].m_position = Vec3( WORLD_SIZE_X, 0.f, 0.f);
	pauseVerts[4].m_position = Vec3( 0.f, 0.f, 0.f);
	pauseVerts[5].m_position = Vec3( 0.f, WORLD_SIZE_Y, 0.f);

	for (int pauseVertIndex = 0; pauseVertIndex < 6; pauseVertIndex++)
	{
		pauseVerts[pauseVertIndex].m_color = Rgba8(128,128,128,100);
	}

	std::vector<Vertex> pauseTextVerts;
	AddVertsForTextTriangles2D(pauseTextVerts, "PAUSED", Vec2(WORLD_CENTER_X - 20.f, WORLD_CENTER_Y + 10.f), 10.f, Rgba8(255, 255, 255));

	g_engine->m_render->DrawVertexArray(6, pauseVerts);
	g_engine->m_render->DrawVertexArray(static_cast<int>(pauseTextVerts.size()), pauseTextVerts.data());
}

void Game::RenderWinOverlay() const
{
	Vertex trophyVerts[18];

	//BASE
	trophyVerts[0].m_position = Vec3(-2.f, -1.f, 0.f);
	trophyVerts[1].m_position = Vec3(2.f, -1.f, 0.f);
	trophyVerts[2].m_position = Vec3(-2.f, 1.f, 0.f);

	trophyVerts[3].m_position = Vec3(2.f, 1.f, 0.f);
	trophyVerts[4].m_position = Vec3(-2.f, 1.f, 0.f);
	trophyVerts[5].m_position = Vec3(2.f, -1.f, 0.f);

	//TROPHY 
	trophyVerts[6].m_position = Vec3(-2.f, 1.f, 0.f);
	trophyVerts[7].m_position = Vec3(0.f, 4.f, 0.f);
	trophyVerts[8].m_position = Vec3(2.f, 1.f, 0.f);

	trophyVerts[9].m_position = Vec3(0.f, 4.f, 0.f);
	trophyVerts[10].m_position = Vec3(-3.f, 6.f, 0.f);
	trophyVerts[11].m_position = Vec3(3.f, 6.f, 0.f);

	trophyVerts[12].m_position = Vec3(-3.f, 6.f, 0.f);
	trophyVerts[13].m_position = Vec3(-3.f, 9.f, 0.f);
	trophyVerts[14].m_position = Vec3(3.f, 6.f, 0.f);

	trophyVerts[15].m_position = Vec3(3.f, 6.f, 0.f);
	trophyVerts[16].m_position = Vec3(-3.f, 9.f, 0.f);
	trophyVerts[17].m_position = Vec3(3.f, 9.f, 0.f);

	for (int i = 0; i < 6; i++)
	{
		trophyVerts[i].m_color = Rgba8(150, 75, 0);
	}
	for (int i = 6; i < 18; i++)
	{
		trophyVerts[i].m_color = Rgba8(255, 255, 0);
	}
	TransformVertexArrayXY3D(18, trophyVerts, 7.f, 0.f, Vec2(SCREEN_SIZE_X, 30.f));

	std::vector<Vertex> winTextVerts;
	AddVertsForTextTriangles2D(winTextVerts, "YOU'RE WINNER", Vec2(SCREEN_SIZE_X - 44.5f, 5.f), 10.f, Rgba8(255, 255, 0));
	AddVertsForTextTriangles2D(winTextVerts, "YOU'RE WINNER", Vec2(SCREEN_SIZE_X - 45.f, 5.f), 10.f, Rgba8(255, 170, 0));

	g_engine->m_render->DrawVertexArray(18, trophyVerts);
	g_engine->m_render->DrawVertexArray(static_cast<int>(winTextVerts.size()), winTextVerts.data());
}

void Game::GenerateStarfield()
{
	for (int starNumber = 0; starNumber < MAX_STARS; ++starNumber)
	{
		float starX = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_X);
		float starY = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_Y);

		Vertex& v1 = m_starVerts[3 * starNumber + 0];
		Vertex& v3 = m_starVerts[3 * starNumber + 1];
		Vertex& v2 = m_starVerts[3 * starNumber + 2];

		v1.m_position = Vec3(starX + 0.f, starY + 0.f, 0.f);
		v2.m_position = Vec3(starX + 0.75f, starY + 0.f, 0.f);
		v3.m_position = Vec3(starX + 0.5f, starY + 0.5f, 0.f);
	}
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// WAVE HANDLING
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::IsReadyToStartNextWave()
{
	int numLivingEnemies = GetNumLivingEnemies();
	/*int numDebris = GetNumDebris();*/
	return (numLivingEnemies == 0 /*&& numDebris == 0*/);
}


void Game::StartNextWave()
{
	if(m_gamestate == GAMESTATE_ATTRACT)
	{
		return;
	}

	if (m_gamestate == GAMESTATE_SHOP)
	{
		return;
	}

	int numAsteroidsPerWave[NUM_WAVES] = {5,5,10,10,15,15};
	int numBeetlesPerWave[NUM_WAVES] = {5,8,8,14,14,18};
	int numWaspsPerWave[NUM_WAVES] = {0,1,2,2,3,3};
	int numEnemyShipsPerWave[NUM_WAVES] = {0,0,1,3,4,5};

	int numAsteroidsToSpawn = numAsteroidsPerWave[m_wave];
	int numBeetlestoSpawn = numBeetlesPerWave[m_wave];
	int numWaspstoSpawn = numWaspsPerWave[m_wave];
	int numEnemyShipsToSpawn = numEnemyShipsPerWave[m_wave];

	if(m_wave < NUM_WAVES)
	{
		for (int i = 0; i < numAsteroidsToSpawn; ++i)
		{
			SpawnNewAsteroid();
		}
		for (int i = 0; i < numBeetlestoSpawn; ++i)
		{
			SpawnNewBeetle();
		}
		for (int i = 0; i < numWaspstoSpawn; ++i)
		{
			SpawnNewWasp();
		}
		for (int i = 0; i < numEnemyShipsToSpawn; ++i)
		{
			SpawnNewEnemyShip();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	ENTITY NUMBER/INFO REPORTING
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::BeetlesCloseToBase() const
{
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		if (m_beetles[beetleIndex] != nullptr && !(m_beetles[beetleIndex]->m_isDead))
		{
			//Base and Beetle Collision
			if (GetDistance2D(m_beetles[beetleIndex]->m_position, Vec2(0.f,WORLD_SIZE_Y)) < BEETLE_DETECTION_RADIUS)
			{
				return true;
			}
		}
	}
	return false;
}

int Game::GetNumLivingEnemies()
{
	int numLivingEnemies = 0;

	for(int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		if (m_wasps[waspIndex] != nullptr && !(m_wasps[waspIndex]->m_isDead))
		{
			numLivingEnemies += 1;
		}
	}

	for(int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		if (m_beetles[beetleIndex] != nullptr && !(m_beetles[beetleIndex]->m_isDead))
		{
			numLivingEnemies += 1;
		}
	}

	for (int shipIndex = 0; shipIndex < MAX_ENEMY_SHIPS; shipIndex++)
	{
		if (m_enemyShips[shipIndex] != nullptr && !(m_enemyShips[shipIndex]->m_isDead))
		{
			numLivingEnemies += 1;
		}
	}

	return numLivingEnemies;
}


int Game::GetNumDebris()
{
	int numDebris = 0;

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		if (m_debris[debrisIndex] != nullptr && !(m_debris[debrisIndex]->m_isDead))
		{
			numDebris += 1;
		}
	}
	
	return numDebris;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ENTITY DELETION
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::DeleteGarbage()
{
	for (int garbageIndex = 0; garbageIndex < MAX_BULLETS; ++garbageIndex)
	{
		if (m_bullets[garbageIndex] != nullptr && m_bullets[garbageIndex]->m_isGarbage)
		{
			delete m_bullets[garbageIndex];
			m_bullets[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_BULLETS; ++garbageIndex)
	{
		if (m_enemyBullets[garbageIndex] != nullptr && m_enemyBullets[garbageIndex]->m_isGarbage)
		{
			delete m_enemyBullets[garbageIndex];
			m_enemyBullets[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_ASTEROIDS; ++garbageIndex)
	{
		if (m_asteroids[garbageIndex] != nullptr && m_asteroids[garbageIndex]->m_isGarbage)
		{
			delete m_asteroids[garbageIndex];
			m_asteroids[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_BEETLES; ++garbageIndex)
	{
		if (m_beetles[garbageIndex] != nullptr && m_beetles[garbageIndex]->m_isGarbage)
		{
			delete m_beetles[garbageIndex];
			m_beetles[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_WASPS; ++garbageIndex)
	{
		if (m_wasps[garbageIndex] != nullptr && m_wasps[garbageIndex]->m_isGarbage)
		{
			delete m_wasps[garbageIndex];
			m_wasps[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_ENEMY_SHIPS; ++garbageIndex)
	{
		if (m_enemyShips[garbageIndex] != nullptr && m_enemyShips[garbageIndex]->m_isGarbage)
		{
			delete m_enemyShips[garbageIndex];
			m_enemyShips[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_DEBRIS; ++garbageIndex)
	{
		if (m_debris[garbageIndex] != nullptr && m_debris[garbageIndex]->m_isGarbage)
		{
			delete m_debris[garbageIndex];
			m_debris[garbageIndex] = nullptr;
		}
	}
}


void Game::DeleteAllEntities()
{
	for (int garbageIndex = 0; garbageIndex < MAX_BULLETS; ++garbageIndex)
	{
		if (m_bullets[garbageIndex] != nullptr)
		{
			delete m_bullets[garbageIndex];
			m_bullets[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_BULLETS; ++garbageIndex)
	{
		if (m_enemyBullets[garbageIndex] != nullptr)
		{
			delete m_enemyBullets[garbageIndex];
			m_enemyBullets[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_ASTEROIDS; ++garbageIndex)
	{
		if (m_asteroids[garbageIndex] != nullptr)
		{
			delete m_asteroids[garbageIndex];
			m_asteroids[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_BEETLES; ++garbageIndex)
	{
		if (m_beetles[garbageIndex] != nullptr)
		{
			delete m_beetles[garbageIndex];
			m_beetles[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_WASPS; ++garbageIndex)
	{
		if (m_wasps[garbageIndex] != nullptr)
		{
			delete m_wasps[garbageIndex];
			m_wasps[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_ENEMY_SHIPS; ++garbageIndex)
	{
		if (m_enemyShips[garbageIndex] != nullptr)
		{
			delete m_enemyShips[garbageIndex];
			m_enemyShips[garbageIndex] = nullptr;
		}
	}

	for (int garbageIndex = 0; garbageIndex < MAX_DEBRIS; ++garbageIndex)
	{
		if (m_debris[garbageIndex] != nullptr)
		{
			delete m_debris[garbageIndex];
			m_debris[garbageIndex] = nullptr;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------
// INPUT
//---------------------------------------------------------------------------------------------------------------------------------
void Game::UpdateFromKeyboard()
{
	InputSystem* input = g_engine->m_input;

	if (input->WasKeyJustPressed(' ') && m_gamestate == GAMESTATE_ATTRACT)
	{
		g_engine->m_audio->StartSound(m_testSound);
		m_gamestate = GAMESTATE_PLAYING;
		gameMusic = g_engine->m_audio->StartSound(m_gameMusic, true, 0.25f, 0.0f, 1.0f, false);
	}

	if (input->WasKeyJustPressed('I') && !g_theApp->m_isPaused && m_gamestate != GAMESTATE_ATTRACT)
	{
		DeleteAllEntities();
	}

	if (input->WasKeyJustPressed(KEYCODE_F1))
	{
		g_engine->m_audio->StartSound(m_testSound);
		g_debugDraw = !g_debugDraw;
	}

	if (input->WasKeyJustPressed(KEYCODE_ESC) && m_gamestate == GAMESTATE_ATTRACT)
	{
		g_theApp->SetIsQuitting();
	}

	if (input->WasKeyJustPressed(KEYCODE_ESC) && m_gamestate != GAMESTATE_ATTRACT)
	{
		g_engine->m_audio->StopSound(gameMusic);
		m_gamestate = GAMESTATE_ATTRACT;
	}

	if (input->WasKeyJustPressed('M') && m_gamestate == GAMESTATE_SHOP)
	{
		m_gamestate = GAMESTATE_PLAYING;
	}
}

void Game::UpdateFromController()
{
	XboxController const& controller = g_engine->m_input->GetController(0);

	if (controller.WasButtonJustPressed(XboxButtonID::START) && (m_gamestate == GAMESTATE_PLAYING || m_gamestate == GAMESTATE_SHOP))
	{
		g_theApp->m_isPaused = !g_theApp->m_isPaused;
	}
	if (controller.WasButtonJustPressed(XboxButtonID::START) && m_gamestate == GAMESTATE_ATTRACT)
	{
		g_engine->m_audio->StartSound(m_testSound);
		m_gamestate = GAMESTATE_PLAYING;
		gameMusic = g_engine->m_audio->StartSound(m_gameMusic, true, 0.25f, 0.0f, 1.0f, false);
	}
	if (controller.WasButtonJustPressed(XboxButtonID::B) && m_gamestate == GAMESTATE_ATTRACT)
	{
		g_theApp->SetIsQuitting();
	}
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// GAMESTATE HANDLING
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::LoseGameHandling()
{
	if ((m_lives < 0 && GetNumDebris() == 0) || (m_baseHealth <= 0.f && GetNumDebris() == 0) || (m_wave >= NUM_WAVES && GetNumDebris() == 0))
	{
		g_engine->m_audio->StopSound(gameMusic);
		delete g_theApp->m_game;
		g_theApp->m_game = new Game();
	}
}

void Game::StartWaveOrShopHandling()
{
	if (m_gamestate == GAMESTATE_SHOP)
	{
		UpdateShop();
	}

	if (m_wave < 0 && m_gamestate != GAMESTATE_ATTRACT)
	{
		DeleteAllEntities();
	}

	if (IsReadyToStartNextWave() && m_wave < NUM_WAVES)
	{
		bool justLeftShopThisFrame = (m_prevState == GAMESTATE_SHOP && m_gamestate == GAMESTATE_PLAYING);

		if ((m_wave == 0 || m_wave == 2 || m_wave == 4) && !justLeftShopThisFrame)
		{
			m_gamestate = GAMESTATE_SHOP;
		}
		if (m_wave < NUM_WAVES && m_gamestate == GAMESTATE_PLAYING)
		{
			++m_wave;
			StartNextWave();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SPAWNING
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Asteroid* Game::SpawnNewAsteroid()
{
	for(int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex)
	{
		if (m_asteroids[asteroidIndex] == nullptr)
		{
			Vec2 pos = GetRandomPositionOffScreen(ASTEROID_COSMETIC_RADIUS);
			m_asteroids[asteroidIndex] = new Asteroid(this, pos, Vec2(0.f,0.f));
			m_asteroids[asteroidIndex]->m_orientationDegrees = g_rng->RollRandomFloatInRange(0.f, 360.f);
			return m_asteroids[asteroidIndex];
		}
	}

	ERROR_RECOVERABLE("Asteroids array is full");
	return nullptr;
}


Beetle* Game::SpawnNewBeetle()
{
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex)
	{
		if (m_beetles[beetleIndex] == nullptr)
		{
			Vec2 pos = GetRandomPositionOffScreen(BEETLE_COSMETIC_RADIUS);
			m_beetles[beetleIndex] = new Beetle(this, pos, Vec2(0.f, 0.f));
			m_beetles[beetleIndex]->m_orientationDegrees = m_ship1->m_orientationDegrees;
			return m_beetles[beetleIndex];
		}
	}

	ERROR_RECOVERABLE("Beetle array is full");
	return nullptr;
}


Wasp* Game::SpawnNewWasp()
{
	for (int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex)
	{
		if (m_wasps[waspIndex] == nullptr)
		{
			Vec2 pos = GetRandomPositionOffScreen(WASP_COSMETIC_RADIUS);
			m_wasps[waspIndex] = new Wasp(this, pos, Vec2(0.f, 0.f));
			m_wasps[waspIndex]->m_orientationDegrees = m_ship1->m_orientationDegrees;
			return m_wasps[waspIndex];
		}
	}

	ERROR_RECOVERABLE("Wasps array is full");
	return nullptr;
}

EnemyShip* Game::SpawnNewEnemyShip()
{
	for (int shipIndex = 0; shipIndex < MAX_ENEMY_SHIPS; ++shipIndex)
	{
		if (m_enemyShips[shipIndex] == nullptr)
		{
			Vec2 pos = GetRandomPositionOffScreen(PLAYER_SHIP_COSMETIC_RADIUS);
			m_enemyShips[shipIndex] = new EnemyShip(this, pos, Vec2(0.f, 0.f));
			m_enemyShips[shipIndex]->m_orientationDegrees = m_ship1->m_orientationDegrees;
			return m_enemyShips[shipIndex];
		}
	}

	ERROR_RECOVERABLE("Enemy Ship array is full");
	return nullptr;
}


Debris* Game::SpawnNewDebris(Vec2 pos, Vec2 vel, float radius, Rgba8 color, [[maybe_unused]]float lifetime)
{
	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex)
	{
		if (m_debris[debrisIndex] == nullptr)
		{
			m_debris[debrisIndex]  = new Debris(this, pos, vel, radius, color);
			m_debris[debrisIndex]->m_velocity = vel;
			return m_debris[debrisIndex];
		}
	}

	ERROR_RECOVERABLE("Debris array is full");
	return nullptr;
}


void Game::SpawnNewDebrisCluster(int count, Vec2 pos, Vec2 clusterVelocity, float radius, Rgba8 color, float minLifetimeSeconds, float maxLifetimeSeconds) 
{
	for (int i = 0; i < count; ++i)
	{
		float heading = g_rng->RollRandomFloatInRange( 0.f, 360.f );
		float speed = g_rng->RollRandomFloatInRange( 30.f, 40.f );
		Vec2 localVelocity = Vec2::MakeFromPolarDegrees( heading, speed );
		Vec2 worldVelocity = clusterVelocity + localVelocity;
		float lifetimeSeconds = g_rng->RollRandomFloatInRange(minLifetimeSeconds, maxLifetimeSeconds);
		SpawnNewDebris(pos, worldVelocity, radius, color, lifetimeSeconds);
	}
}


Vec2 Game::GetRandomPositionOffScreen(float radius) const
{
	int edge = g_rng->RollRandomIntInRange(0, 1);
	Vec2 pos;
// 	if (edge == 0) // Left
// 	{
// 		pos.x = -radius;
// 		pos.y = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_Y);
// 	}
	if (edge == 1) // Right
	{
		pos.x = g_rng->RollRandomFloatInRange(WORLD_SIZE_X + radius, WORLD_SIZE_X + 50.f);
		pos.y = g_rng->RollRandomFloatInRange(-WORLD_SIZE_Y/2.f, WORLD_SIZE_Y);
	}
// 	else if (edge == 2) //Top
// 	{
// 		pos.x = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_X);
// 		pos.y = WORLD_SIZE_Y + radius;
// 	}
	else //Bottom
	{
		pos.x = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_X + (WORLD_SIZE_X / 2.f));
		pos.y = g_rng->RollRandomFloatInRange(-radius + 50.f, -radius-200.f);
	}

	return pos;
}


Clock* Game::GetClock() const
{
	return m_gameClock;
}

void Game::PrintControlsToConsole()
{
	Rgba8 header(0, 200, 255, 255);
	Rgba8 keyCol(255, 230, 100, 255);
	Rgba8 textCol(200, 200, 200, 255);

	g_engine->m_devConsole->AddLine(header, "=== Game / Debug Keys ===");

	g_engine->m_devConsole->AddLine(keyCol, "P");
	g_engine->m_devConsole->AddLine(textCol, "  Pause / Unpause game clock");

	g_engine->m_devConsole->AddLine(keyCol, "O");
	g_engine->m_devConsole->AddLine(textCol, "  Step single frame (while paused)");

	g_engine->m_devConsole->AddLine(keyCol, "T");
	g_engine->m_devConsole->AddLine(textCol, "  Toggle time scale between 0.1 and 1.0");

	g_engine->m_devConsole->AddLine(keyCol, "set_game_time_scale <scale>");
	g_engine->m_devConsole->AddLine(textCol, "  Console command to set game clock time scale");
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// COOL SOUND B )
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::CreateSound()
{
	m_testSound = g_engine->m_audio->CreateOrGetSound("Data/Audio/TestSound.mp3");
	m_deathSound = g_engine->m_audio->CreateOrGetSound("Data/Audio/lego-yoda-death-sound-effect.mp3");
	m_pewSound = g_engine->m_audio->CreateOrGetSound("Data/Audio/Shoot70.wav");
	m_gameMusic = g_engine->m_audio->CreateOrGetSound("Data/Audio/BreakCore_Space.mp3");
	m_explosion = g_engine->m_audio->CreateOrGetSound("Data/Audio/Boom13.wav");
	m_shipExlposion = g_engine->m_audio->CreateOrGetSound("Data/Audio/shipBoom.wav");
}
