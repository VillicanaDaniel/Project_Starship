#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include <cmath>

EulerAngles::EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees)
	: m_yawDegrees(yawDegrees)
	, m_pitchDegrees(pitchDegrees)
	, m_rollDegrees(rollDegrees)
{
}

void EulerAngles::SetFromText(char const* text)
{
	if (text == nullptr)
	{
		m_yawDegrees = 0.f;
		m_pitchDegrees = 0.f;
		m_rollDegrees = 0.f;
		return;
	}

	Strings parts = SplitStringOnDelimiter(text, ',');

	if (parts.size() != 3)
	{
		m_yawDegrees = 0.f;
		m_pitchDegrees = 0.f;
		m_rollDegrees = 0.f;
		return;
	}

	m_yawDegrees = (float)atof(parts[0].c_str());
	m_pitchDegrees = (float)atof(parts[1].c_str());
	m_rollDegrees = (float)atof(parts[2].c_str());
}

Vec3 EulerAngles::GetForwardDir_IFwd_JLeft_KUp() const
{
	Vec3 f, l, u;
	GetAsVectors_IFwd_JLeft_KUp(f, l, u);
	return f;
}

void EulerAngles::GetAsVectors_IFwd_JLeft_KUp(Vec3& out_forwardBasis, Vec3& out_leftBasis, Vec3& out_upBasis) const
{
	float cy = CosDegrees(m_yawDegrees);
	float sy = SinDegrees(m_yawDegrees);

	float cp = CosDegrees(m_pitchDegrees);
	float sp = SinDegrees(m_pitchDegrees);

	float cr = CosDegrees(m_rollDegrees);
	float sr = SinDegrees(m_rollDegrees);

	// Forward
	out_forwardBasis = Vec3(
		cp * cy,
		cp * sy,
		-sp
	);

	// Left
	out_leftBasis = Vec3(
		sr * sp * cy - cr * sy,
		sr * sp * sy + cr * cy,
		sr * cp
	);

	// Up
	out_upBasis = Vec3(
		cr * sp * cy + sr * sy,
		cr * sp * sy - sr * cy,
		cr * cp
	);
}

Mat44 EulerAngles::GetAsMatrix_IFwd_JLeft_KUp() const
{
	Vec3 f, l, u;
	GetAsVectors_IFwd_JLeft_KUp(f, l, u);

	return Mat44(f, l, u, Vec3(0.f, 0.f, 0.f));
}

//------------------------------------------------------------
void EulerAngles::operator+=(EulerAngles const& anglesToAdd)
{
	m_yawDegrees += anglesToAdd.m_yawDegrees;
	m_pitchDegrees += anglesToAdd.m_pitchDegrees;
	m_rollDegrees += anglesToAdd.m_rollDegrees;
}

EulerAngles Interpolate(EulerAngles const& from, EulerAngles const& to, float lerpFraction)
{
	EulerAngles result;

	float yawDelta = GetShortestAngularDispDegrees(from.m_yawDegrees, to.m_yawDegrees);
	float pitchDelta = GetShortestAngularDispDegrees(from.m_pitchDegrees, to.m_pitchDegrees);
	float rollDelta = GetShortestAngularDispDegrees(from.m_rollDegrees, to.m_rollDegrees);

	result.m_yawDegrees = from.m_yawDegrees + yawDelta * lerpFraction;
	result.m_pitchDegrees = from.m_pitchDegrees + pitchDelta * lerpFraction;
	result.m_rollDegrees = from.m_rollDegrees + rollDelta * lerpFraction;

	return result;
}