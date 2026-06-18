#pragma once
#include "Engine/Math/Vec3.hpp"

struct OBB3
{
public:
	OBB3() = default;

	OBB3(
		Vec3 const& center,
		Vec3 const& iBasis,
		Vec3 const& jBasis,
		Vec3 const& kBasis,
		Vec3 const& halfDimensions
	);

	Vec3 GetLocalPosForWorldPos(Vec3 const& worldPos) const;
	Vec3 GetWorldPosForLocalPos(Vec3 const& localPos) const;
	void GetCornerPoints(Vec3* out_eightCornerWorldPositions) const;

public:
	Vec3 m_center = Vec3::ZERO;

	Vec3 m_iBasis = Vec3(1.f, 0.f, 0.f);
	Vec3 m_jBasis = Vec3(0.f, 1.f, 0.f);
	Vec3 m_kBasis = Vec3(0.f, 0.f, 1.f);

	Vec3 m_halfDimensions = Vec3(0.5f, 0.5f, 0.5f);
};