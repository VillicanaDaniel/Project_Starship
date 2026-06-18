#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"

void Camera::SetOrthographicView(Vec2 const& bottomLeft, Vec2 const& topRight, float nearZ, float farZ)
{
	m_mode = eMode_Orthographic;

	m_orthographicBottomLeft = bottomLeft;
	m_orthographicTopRight = topRight;
	m_orthographicNear = nearZ;
	m_orthographicFar = farZ;
}

void Camera::SetPerspectiveView(float aspect, float fovYDegrees, float nearZ, float farZ)
{
	m_mode = eMode_Perspective;

	m_perspectiveAspect = aspect;
	m_perspectiveFOVY = fovYDegrees;
	m_perspectiveNear = nearZ;
	m_perspectiveFar = farZ;
}

void Camera::SetPositionAndOrientation(Vec3 const& position, EulerAngles const& orientation)
{
	m_position = position;
	m_orientation = orientation;
}

void Camera::SetPosition(Vec3 const& position)
{
	m_position = position;
}

Vec3 Camera::GetPosition() const
{
	return m_position;
}

void Camera::SetOrientation(EulerAngles const& orientation)
{
	m_orientation = orientation;
}

EulerAngles Camera::GetOrientation() const
{
	return m_orientation;
}

Mat44 Camera::GetOrthographicMatrix() const
{
	return Mat44::MakeOrthoProjection(
		m_orthographicBottomLeft.x,
		m_orthographicTopRight.x,
		m_orthographicBottomLeft.y,
		m_orthographicTopRight.y,
		m_orthographicNear,
		m_orthographicFar
	);
}

Mat44 Camera::GetPerspectiveMatrix() const
{
	return Mat44::MakePerspectiveProjection(
		m_perspectiveFOVY,
		m_perspectiveAspect,
		m_perspectiveNear,
		m_perspectiveFar
	);
}

Mat44 Camera::GetProjectionMatrix() const
{
	return (m_mode == eMode_Perspective) ? GetPerspectiveMatrix() : GetOrthographicMatrix();
}

Mat44 Camera::GetCameraToWorldTransform() const
{
	Mat44 rot = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

	Mat44 camToWorld = Mat44::MakeTranslation3D(m_position);
	camToWorld.Append(rot);
	return camToWorld;
}

Mat44 Camera::GetWorldToCameraTransform() const
{
	Mat44 camToWorld = GetCameraToWorldTransform();
	return camToWorld.GetOrthonormalInverse();
}

void Camera::SetCameraToRenderTransform(Mat44 const& m)
{
	m_cameraToRenderTransform = m;
}

Mat44 Camera::GetCameraToRenderTransform() const
{
	return m_cameraToRenderTransform;
}

Mat44 Camera::GetRenderToClipTransform() const
{
	return GetProjectionMatrix();
}

Vec2 Camera::GetOrthographicBottomLeft() const
{
	return m_orthographicBottomLeft;
}

Vec2 Camera::GetOrthographicTopRight() const
{
	return m_orthographicTopRight;
}

void Camera::Translate2D(Vec2 const& translation)
{
	m_position.x += translation.x;
	m_position.y += translation.y;
}