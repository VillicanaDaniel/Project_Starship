#include "Engine/Math/Plane3.hpp"

Plane3::Plane3(Vec3 const& normal, float distFromOrigin)
	: m_normal(normal.GetNormalized())
	, m_distFromOrigin(distFromOrigin)
{
}