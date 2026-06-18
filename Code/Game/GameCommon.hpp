#pragma once
struct Vec2;
struct Rgba8;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// STARS
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr int MAX_STARS = 1000;
constexpr int VERTS_PER_STAR = 3;
constexpr int NUM_STAR_VERTS = MAX_STARS * VERTS_PER_STAR;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ENTITY MAXES
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr int MAX_ASTEROIDS = 100;
constexpr int MAX_BULLETS = 150;
constexpr int MAX_BEETLES = 30;
constexpr int MAX_WASPS = 30;
constexpr int MAX_ENEMY_SHIPS = 10;
constexpr int MAX_LIVES = 3;
constexpr int MAX_DEBRIS = 1000;

constexpr int NUM_WAVES = 6;
constexpr int DEBRIS_LIFETIME_SECONDS = 2;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// WORLD AND SCREEN SIZING
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr float WORLD_SIZE_X = 500.f;
constexpr float WORLD_SIZE_Y = 250.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float SCREEN_SIZE_X = 250.f;
constexpr float SCREEN_SIZE_Y = 125.f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SHOP ITEMS
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr float SHOP_ITEM_RADIUS = 10.f;
constexpr float SHOP_TEXT_SIZE = 7.f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ASTEROIDS
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr float ASTEROID_SPEED = 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 8.0f;
constexpr float ASTEROID_COSMETIC_RADIUS = 10.0f;
constexpr int ASTEROID_HEALTH = 5;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// BULLETS
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 200.f;
constexpr float BULLET_PHYSICS_RADIUS = 1.3f;
constexpr float BULLET_COSMETIC_RADIUS = 4.0f;
constexpr int NUM_BULLET_VERTS = 6;
constexpr float ENEMY_BULLET_SPEED = 100.f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// PLAYER BASE
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr float MAX_BASE_HEALTH = 10.f;
constexpr float BASE_PHYSICS_RADIUS = 50.f;
constexpr float BEETLE_DETECTION_RADIUS = 220.f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// PLAYERSHIP
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr float PLAYER_SHIP_ACCELERATION = 300.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 3.f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 5.5f;
constexpr int NUM_SHIP_VERTS = 33;

constexpr float PLAYER_SHIP_FIRE_SPEED = 0.12f;
constexpr float PLAYER_SHIP_SHIELD_RADIUS = 6.f;
constexpr float PLAYER_SHIP_SHIELD_COOLDOWN = 6.f;
constexpr float INVULNERABILITY_TIMER = 2.f;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ENEMY SHIP
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr float ENEMY_SHIP_FIRE_SPEED = 0.5f;
constexpr float ENEMY_SHIP_TURN_SPEED = 100.f;
constexpr float ENEMY_SHIP_PHYSICS_RADIUS = 4.5f;
constexpr float ENEMY_SHIP_COSMETIC_RADIUS = 5.f;
constexpr float ENEMY_SHIP_SHOOT_DISTANCE = 60.f;
constexpr int ENEMY_SHIP_HEALTH = 4;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// BEETLE
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr float BEETLE_PHYSICS_RADIUS = 6.f;
constexpr float BEETLE_COSMETIC_RADIUS = 8.f;
constexpr float BEETLE_SPEED = 20.f;
constexpr int BEETLE_HEALTH = 5;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// WASP
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
constexpr float WASP_PHYSICS_RADIUS = 1.6f;
constexpr float WASP_COSMETIC_RADIUS = 2.2360679775f;
constexpr float WASP_WING_TIMER = 0.5f;
constexpr float WASP_SPEED = 100.f;
constexpr int WASP_HEALTH = 2;


void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& center, Vec2 end,float thickness, Rgba8 const& color);