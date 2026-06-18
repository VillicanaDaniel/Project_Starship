#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex.hpp"


class Wasp : public Entity
{

public:
	Wasp(Game* Owner, Vec2 const& startingPosition, Vec2 const& startingVelocity);
	~Wasp() = default;

	virtual void Update() override;
	virtual void Render() const override;
	virtual void setDead() override;

	bool isCloseToPlayer;

	float m_wingTimer = WASP_WING_TIMER;

private:
	void InitializeLocalVerts();

private:
	Vertex m_localVerts[15];
};