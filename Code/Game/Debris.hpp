#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"

constexpr int NUM_DEBRIS_TRIS = 8;
constexpr int NUM_DEBRIS_SIDES = 8;
constexpr int NUM_DEBRIS_VERTS = NUM_DEBRIS_SIDES * 3;
class Debris : public Entity
{

public:
	Debris(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity, float radius, Rgba8 color);
	~Debris() = default;

	Vec2 initial_orientation;
	float angular_velocity;

	Rgba8 debris_color;
	float m_timeoutTimer = DEBRIS_LIFETIME_SECONDS;
	float alphaValue;


	virtual void Update() override;
	virtual void Render() const override;

private:
	void InitializeLocalVerts();

private:
	Vertex m_localVerts[NUM_DEBRIS_TRIS * 3];




};