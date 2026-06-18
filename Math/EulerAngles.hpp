#pragma once

struct Vec3;
struct Mat44;

struct EulerAngles
{
public:
	EulerAngles() = default;
	EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees);
	void SetFromText(char const* text);

	Vec3 GetForwardDir_IFwd_JLeft_KUp() const;
	void GetAsVectors_IFwd_JLeft_KUp(Vec3& out_forwardBasis, Vec3& out_leftBasis, Vec3& out_upBasis) const;
	Mat44 GetAsMatrix_IFwd_JLeft_KUp() const;

	void operator+=(EulerAngles const& anglesToAdd);

public:
	float m_yawDegrees = 0.f;
	float m_pitchDegrees = 0.f;
	float m_rollDegrees = 0.f;
};

EulerAngles Interpolate(EulerAngles const& from, EulerAngles const& to, float lerpFraction);

