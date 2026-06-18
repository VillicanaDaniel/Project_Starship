#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"


class Camera
{
public:

	enum Mode
	{
		eMode_Orthographic,
		eMode_Perspective,
		eMode_Count
	};


public:

	Camera() = default;
	~Camera() = default;

	void SetOrthographicView(Vec2 const& bottomLeft, Vec2 const& topRight, float nearZ = 0.0f, float farZ = 1.0f);
	void SetPerspectiveView(float aspect, float fovYDegrees, float nearZ, float farZ);
	void SetPositionAndOrientation(Vec3 const& position, EulerAngles const& orientation);

	void SetPosition(Vec3 const& position);
	Vec3 GetPosition() const;

	void SetOrientation(EulerAngles const& orientation);
	EulerAngles GetOrientation() const;

	Mat44 GetCameraToWorldTransform() const;
	Mat44 GetWorldToCameraTransform() const;

	void SetCameraToRenderTransform(Mat44 const& m);
	Mat44 GetCameraToRenderTransform() const;

	Mat44 GetRenderToClipTransform() const;

	Mat44 GetOrthographicMatrix() const;
	Mat44 GetPerspectiveMatrix() const;

	Mat44 GetProjectionMatrix() const;

	Vec2 GetOrthographicBottomLeft() const;
	Vec2 GetOrthographicTopRight() const;

	void Translate2D(Vec2 const& translation);

	Vec3 m_position;
	EulerAngles m_orientation;


protected:
	Mode m_mode = eMode_Orthographic;

	Vec2  m_orthographicBottomLeft;
	Vec2  m_orthographicTopRight;
	float m_orthographicNear = 0.0f;
	float m_orthographicFar = 1.0f;

	float m_perspectiveAspect = 1.0f;
	float m_perspectiveFOVY = 60.0f;
	float m_perspectiveNear = 0.1f;
	float m_perspectiveFar = 1000.0f;

	Mat44 m_cameraToRenderTransform;
};