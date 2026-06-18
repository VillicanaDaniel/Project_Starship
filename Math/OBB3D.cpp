#include "Engine/Math/OBB3D.hpp"
#include "Engine/Math/MathUtils.hpp"

OBB3::OBB3(
	Vec3 const& center,
	Vec3 const& iBasis,
	Vec3 const& jBasis,
	Vec3 const& kBasis,
	Vec3 const& halfDimensions)
	: m_center(center)
	, m_iBasis(iBasis.GetNormalized())
	, m_jBasis(jBasis.GetNormalized())
	, m_kBasis(kBasis.GetNormalized())
	, m_halfDimensions(halfDimensions)
{
}

Vec3 OBB3::GetLocalPosForWorldPos(Vec3 const& worldPos) const
{
	Vec3 displacement = worldPos - m_center;

	return Vec3(
		DotProduct3D(displacement, m_iBasis),
		DotProduct3D(displacement, m_jBasis),
		DotProduct3D(displacement, m_kBasis)
	);
}

Vec3 OBB3::GetWorldPosForLocalPos(Vec3 const& localPos) const
{
	return m_center +
		m_iBasis * localPos.x +
		m_jBasis * localPos.y +
		m_kBasis * localPos.z;
}

void OBB3::GetCornerPoints(Vec3* out_eightCornerWorldPositions) const
{
	Vec3 i = m_iBasis * m_halfDimensions.x;
	Vec3 j = m_jBasis * m_halfDimensions.y;
	Vec3 k = m_kBasis * m_halfDimensions.z;

	out_eightCornerWorldPositions[0] = m_center - i - j - k;
	out_eightCornerWorldPositions[1] = m_center + i - j - k;
	out_eightCornerWorldPositions[2] = m_center - i + j - k;
	out_eightCornerWorldPositions[3] = m_center + i + j - k;

	out_eightCornerWorldPositions[4] = m_center - i - j + k;
	out_eightCornerWorldPositions[5] = m_center + i - j + k;
	out_eightCornerWorldPositions[6] = m_center - i + j + k;
	out_eightCornerWorldPositions[7] = m_center + i + j + k;
}