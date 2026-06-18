#pragma once 
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

class Game;
class Bullet;

class PlayerShip : public Entity
{
public:
	PlayerShip( Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity);
	~PlayerShip() = default;

	virtual void Update() override;
	virtual void Render() const override;
	virtual void setDead() override;

	void Respawn();

	Game* m_game = nullptr;

	Vertex m_localVerts[NUM_SHIP_VERTS];

	float m_thrustFraction;
	float m_bulletTimer;

	bool m_hasDash;
	bool m_hasShield;
	bool m_hasWeaponUpgrade;

	bool m_isInvulnerable;
	float m_InvulnerableTimer;

	float m_dashTimer;
	float m_dashCooldown;

	float m_shieldHealth;
	float m_shieldCooldown;

	bool m_isShooting;

private:
	void InitializeLocalVerts();

	void UpdateFromKeyboard();
	void UpdateFromController();

	void DashHandling();
	void InvulnerabilityHandling();
	void ShieldHandling();

	void SlowIfShooting();
	void BounceOffWalls();
	void BounceOffBase();

	void RenderShieldHandling() const;

	Bullet* SpawnNewBullet();
};