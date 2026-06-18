#pragma once
#include "Engine/Math/Vec2.hpp"

class Game;

class Entity
{
public:
	Game* m_game = nullptr;
	Vec2 m_position;
	Vec2 m_velocity;
	float m_orientationDegrees;
	float m_angularVelocity;
	float m_physicsRadius;
	float m_cosmeticRadius;
	bool m_isDead = false;
	bool m_isGarbage = false;
	int m_health = 1;

public:
	Entity( Game* owner, Vec2 const& startingPosition, Vec2 const& startingVelocity, int health = 1 );
	virtual ~Entity() = default;

	virtual void Update() = 0;
	virtual void Render() const = 0;
	virtual void RenderDebug() const;
	virtual void TakeDamage(int damage);
	virtual void setDead();

	Vec2 GetForwardNormal() const;
	bool IsOffScreen() const;
};