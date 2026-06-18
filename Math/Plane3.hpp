#pragma once
#include "Engine/Math/Vec3.hpp"

struct Plane3
{
public:
	Plane3() = default;
	Plane3(Vec3 const& normal, float distFromOrigin);

public:
	Vec3 m_normal = Vec3(0.f, 0.f, 1.f);
	float m_distFromOrigin = 0.f;
};