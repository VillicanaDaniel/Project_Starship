#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"

class EnemyBullet;

class EnemyShip : public Entity
{

public:
	EnemyShip(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity);
	~EnemyShip() = default;

	virtual void Update() override;
	virtual void Render() const override;
	virtual void setDead() override;

	EnemyBullet* SpawnNewBullet();

	float m_bulletTimer;

	Vertex m_localVerts[NUM_SHIP_VERTS];
private:
	void InitializeLocalVerts();
};