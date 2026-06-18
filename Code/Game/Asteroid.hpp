#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec3.hpp"

constexpr int NUM_ASTEROID_TRIS = 16;
constexpr int NUM_ASTEROID_SIDES = 16;
constexpr int NUM_ASTEROID_VERTS = NUM_ASTEROID_SIDES * 3;


class Asteroid : public Entity
{

public:
	Asteroid(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity);
	~Asteroid() = default;

	Vec2 initial_orientation;
	float angular_velocity;


	virtual void Update() override;
	virtual void Render() const override;
	virtual void setDead() override;
	virtual void TakeDamage(int damage) override;

private:
	void InitializeLocalVerts();
	void WrapScreen();


private:
	Vertex m_localVerts[NUM_ASTEROID_VERTS];

};