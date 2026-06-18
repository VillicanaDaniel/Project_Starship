#pragma once 
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"


class Bullet : public Entity
{

public:
	Bullet(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity);
	~Bullet() = default;
	virtual void Update() override;
	virtual void Render() const override;
	virtual void setDead() override;

	bool IsOffView();

private:
	void InitializeLocalVerts();


private:
	Vertex m_localVerts[NUM_BULLET_VERTS];

};