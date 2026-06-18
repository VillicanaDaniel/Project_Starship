#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Vertex.hpp"

class PlayerShip;
class Camera;
class Bullet;
class Asteroid;
class Beetle;
class Wasp;
class EnemyShip;
class Debris;
class EnemyBullet;
class Clock;

class Game
{
public:
	Game();
	~Game();

	void Update();
	void Render() const;

	void UpdateEntities();
	void UpdateShop();
	void UpdateCameras();
	void AddScreenShake(float additionalShake);
	
	void RenderAttractMode() const;
	void RenderEntities() const;
	void DebugRenderEntities() const;
	void RenderBase() const;
	void RenderUI() const;
	void RenderShop() const;
	void RenderPaused() const;
	void RenderWinOverlay() const;
	void GenerateStarfield();

	bool IsReadyToStartNextWave();
	void StartNextWave();

	bool BeetlesCloseToBase() const;
	int GetNumLivingEnemies();
	int GetNumDebris();
	
	void DeleteGarbage();
	void DeleteAllEntities();

	void UpdateFromKeyboard();
	void UpdateFromController();

	void LoseGameHandling();
	void StartWaveOrShopHandling();

	Asteroid* SpawnNewAsteroid();
	Beetle* SpawnNewBeetle();
	Wasp* SpawnNewWasp();
	EnemyShip* SpawnNewEnemyShip();
	Debris* SpawnNewDebris( Vec2 pos, Vec2 vel, float radius, Rgba8 color, float lifetime);
	void SpawnNewDebrisCluster( int count, Vec2 pos, Vec2 clusterVelocity, float radius, Rgba8 color, float minLifetimeSeconds, float maxLifetimeSeconds);

	Vec2 GetRandomPositionOffScreen(float radius) const;

	void PrintControlsToConsole();

public:
	PlayerShip* m_ship1 = nullptr;
	Asteroid* m_asteroids[MAX_ASTEROIDS] = {};
	Bullet* m_bullets[MAX_BULLETS] = {};
	EnemyBullet* m_enemyBullets[MAX_BULLETS] = {};
	Beetle* m_beetles[MAX_BEETLES] = {};
	Debris* m_debris[MAX_DEBRIS] = {};
	Wasp* m_wasps[MAX_WASPS] = {};
	EnemyShip* m_enemyShips[MAX_ENEMY_SHIPS] = {};

	Camera* m_worldCamera = nullptr;
	Camera* m_screenCamera = nullptr;

	Vertex m_starVerts[NUM_STAR_VERTS];
	Vertex m_pauseVerts[6];

	bool g_debugDraw = false;

	int m_lives = MAX_LIVES;
	int m_wave = -1;
	int m_bulletDamage = 1;
	float m_baseHealth;
	float m_screenShakeAmount = 0.f;

	Clock* GetClock() const;
	Clock* m_gameClock = nullptr;

	enum GameState
	{
		GAMESTATE_ATTRACT,
		GAMESTATE_PLAYING,
		GAMESTATE_SHOP,
	};

	GameState m_gamestate = GAMESTATE_ATTRACT;
	GameState m_prevState;

	SoundID m_testSound;
	SoundID m_deathSound;
	SoundID m_pewSound;
	SoundID m_gameMusic;
	SoundID m_explosion;
	SoundID m_shipExlposion;
	SoundPlaybackID gameMusic;

	void CreateSound();
};