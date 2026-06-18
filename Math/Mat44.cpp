#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"

Mat44::Mat44()
{
	// Initialize all 16 values
	for (int i = 0; i < 16; i++) {
		m_values[i] = 0.f;
	}

	m_values[Ix] = 1.f;
	m_values[Jy] = 1.f;
	m_values[Kz] = 1.f;
	m_values[Tw] = 1.f;
}

//------------------------------------------------------------------------------------------------
Mat44::Mat44(float const* sixteen)
{
	m_values[Ix] = sixteen[0];
	m_values[Iy] = sixteen[1];
	m_values[Iz] = sixteen[2];
	m_values[Iw] = sixteen[3];

	m_values[Jx] = sixteen[4];
	m_values[Jy] = sixteen[5];
	m_values[Jz] = sixteen[6];
	m_values[Jw] = sixteen[7];

	m_values[Kx] = sixteen[8];
	m_values[Ky] = sixteen[9];
	m_values[Kz] = sixteen[10];
	m_values[Kw] = sixteen[11];

	m_values[Tx] = sixteen[12];
	m_values[Ty] = sixteen[13];
	m_values[Tz] = sixteen[14];
	m_values[Tw] = sixteen[15];
}

Mat44::Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D)
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iz] = 0.f;
	m_values[Iw] = 0.f;

	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jz] = 0.f;
	m_values[Jw] = 0.f;

	m_values[Kx] = 0.f;
	m_values[Ky] = 0.f;
	m_values[Kz] = 1.f;
	m_values[Kw] = 0.f;

	m_values[Tx] = translation2D.x;
	m_values[Ty] = translation2D.y;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;
}

Mat44::Mat44(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D)
{
	m_values[Ix] = iBasis3D.x; m_values[Iy] = iBasis3D.y; m_values[Iz] = iBasis3D.z; m_values[Iw] = 0.f;
	m_values[Jx] = jBasis3D.x; m_values[Jy] = jBasis3D.y; m_values[Jz] = jBasis3D.z; m_values[Jw] = 0.f;
	m_values[Kx] = kBasis3D.x; m_values[Ky] = kBasis3D.y; m_values[Kz] = kBasis3D.z; m_values[Kw] = 0.f;
	m_values[Tx] = translation3D.x; m_values[Ty] = translation3D.y; m_values[Tz] = translation3D.z; m_values[Tw] = 1.f;
}

Mat44::Mat44(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
{
	m_values[Ix] = iBasis4D.x; m_values[Iy] = iBasis4D.y; m_values[Iz] = iBasis4D.z; m_values[Iw] = iBasis4D.w;
	m_values[Jx] = jBasis4D.x; m_values[Jy] = jBasis4D.y; m_values[Jz] = jBasis4D.z; m_values[Jw] = jBasis4D.w;
	m_values[Kx] = kBasis4D.x; m_values[Ky] = kBasis4D.y; m_values[Kz] = kBasis4D.z; m_values[Kw] = kBasis4D.w;
	m_values[Tx] = translation4D.x; m_values[Ty] = translation4D.y; m_values[Tz] = translation4D.z; m_values[Tw] = translation4D.w;
}

Mat44 Mat44::MakeTranslation2D(Vec2 const& translationXY)
{
	Mat44 m;
	m.m_values[Tx] = translationXY.x;
	m.m_values[Ty] = translationXY.y;
	return m;
}

Mat44 Mat44::MakeTranslation3D(Vec3 const& translationXYZ)
{
	Mat44 m;
	m.m_values[Tx] = translationXYZ.x;
	m.m_values[Ty] = translationXYZ.y;
	m.m_values[Tz] = translationXYZ.z;
	return m;
}

Mat44 Mat44::MakeUniformScale2D(float uniformScaleXY)
{
	Mat44 m;
	m.m_values[Ix] = uniformScaleXY;
	m.m_values[Jy] = uniformScaleXY;
	return m;
}

Mat44 Mat44::MakeUniformScale3D(float uniformScaleXYZ)
{
	Mat44 m;
	m.m_values[Ix] = uniformScaleXYZ;
	m.m_values[Jy] = uniformScaleXYZ;
	m.m_values[Kz] = uniformScaleXYZ;
	return m;
}

Mat44 Mat44::MakeNonUniformScale2D(Vec2 const& nonUniformScaleXY)
{
	Mat44 m;
	m.m_values[Ix] = nonUniformScaleXY.x;
	m.m_values[Jy] = nonUniformScaleXY.y;
	return m;
}

Mat44 Mat44::MakeNonUniformScale3D(Vec3 const& nonUniformScaleXYZ)
{
	Mat44 m;
	m.m_values[Ix] = nonUniformScaleXYZ.x;
	m.m_values[Jy] = nonUniformScaleXYZ.y;
	m.m_values[Kz] = nonUniformScaleXYZ.z;
	return m;
}

Mat44 Mat44::MakeZRotationDegrees(float rotationDegreesAboutZ)
{
	float c = CosDegrees(rotationDegreesAboutZ);
	float s = SinDegrees(rotationDegreesAboutZ);

	Mat44 m;
	m.m_values[Ix] = c;
	m.m_values[Iy] = s;
	m.m_values[Iz] = 0.f;

	m.m_values[Jx] = -s;
	m.m_values[Jy] = c;
	m.m_values[Jz] = 0.f;

	m.m_values[Kx] = 0.f;
	m.m_values[Ky] = 0.f;
	m.m_values[Kz] = 1.f;
	return m;
}


Mat44 Mat44::MakeYRotationDegrees(float rotationDegreesAboutY)
{
	float c = CosDegrees(rotationDegreesAboutY);
	float s = SinDegrees(rotationDegreesAboutY);

	Mat44 m; 

	m.m_values[Ix] = c;
	m.m_values[Iy] = 0.f;
	m.m_values[Iz] = -s;

	m.m_values[Jx] = 0.f;
	m.m_values[Jy] = 1.f;
	m.m_values[Jz] = 0.f;

	m.m_values[Kx] = s;
	m.m_values[Ky] = 0.f;
	m.m_values[Kz] = c;

	return m;
}

Mat44 Mat44::MakeXRotationDegrees(float rotationDegreesAboutX)
{
	float c = CosDegrees(rotationDegreesAboutX);
	float s = SinDegrees(rotationDegreesAboutX);

	Mat44 m;
	m.m_values[Ix] = 1.f;
	m.m_values[Iy] = 0.f;
	m.m_values[Iz] = 0.f;

	m.m_values[Jx] = 0.f;
	m.m_values[Jy] = c;
	m.m_values[Jz] = s;

	m.m_values[Kx] = 0.f;
	m.m_values[Ky] = -s;
	m.m_values[Kz] = c;

	return m;
}

Vec2 Mat44::TransformVectorQuantity2D(Vec2 const& vectorQuantityXY) const
{
	return Vec2(
		vectorQuantityXY.x * m_values[Ix] + vectorQuantityXY.y * m_values[Jx],
		vectorQuantityXY.x * m_values[Iy] + vectorQuantityXY.y * m_values[Jy]
	);
}

Vec3 Mat44::TransformVectorQuantity3D(Vec3 const& vectorQuantityXYZ) const
{
	return Vec3(
		vectorQuantityXYZ.x * m_values[Ix] + vectorQuantityXYZ.y * m_values[Jx] + vectorQuantityXYZ.z * m_values[Kx],
		vectorQuantityXYZ.x * m_values[Iy] + vectorQuantityXYZ.y * m_values[Jy] + vectorQuantityXYZ.z * m_values[Ky],
		vectorQuantityXYZ.x * m_values[Iz] + vectorQuantityXYZ.y * m_values[Jz] + vectorQuantityXYZ.z * m_values[Kz]
	);
}

Vec2 Mat44::TransformPosition2D(Vec2 const& positionXY) const
{
	return Vec2(
		positionXY.x * m_values[Ix] + positionXY.y * m_values[Jx] + m_values[Tx],
		positionXY.x * m_values[Iy] + positionXY.y * m_values[Jy] + m_values[Ty]
	);
}

Vec3 Mat44::TransformPosition3D(Vec3 const& positionXYZ) const
{
	return Vec3(
		positionXYZ.x * m_values[Ix] + positionXYZ.y * m_values[Jx] + positionXYZ.z * m_values[Kx] + m_values[Tx],
		positionXYZ.x * m_values[Iy] + positionXYZ.y * m_values[Jy] + positionXYZ.z * m_values[Ky] + m_values[Ty],
		positionXYZ.x * m_values[Iz] + positionXYZ.y * m_values[Jz] + positionXYZ.z * m_values[Kz] + m_values[Tz]
	);
}

Vec4 Mat44::TransformHomogeneous3D(Vec4 const& homogeneousPoint3D) const
{
	return Vec4(
		homogeneousPoint3D.x * m_values[Ix] + homogeneousPoint3D.y * m_values[Jx] + homogeneousPoint3D.z * m_values[Kx] + homogeneousPoint3D.w * m_values[Tx],
		homogeneousPoint3D.x * m_values[Iy] + homogeneousPoint3D.y * m_values[Jy] + homogeneousPoint3D.z * m_values[Ky] + homogeneousPoint3D.w * m_values[Ty],
		homogeneousPoint3D.x * m_values[Iz] + homogeneousPoint3D.y * m_values[Jz] + homogeneousPoint3D.z * m_values[Kz] + homogeneousPoint3D.w * m_values[Tz],
		homogeneousPoint3D.x * m_values[Iw] + homogeneousPoint3D.y * m_values[Jw] + homogeneousPoint3D.z * m_values[Kw] + homogeneousPoint3D.w * m_values[Tw]
	);
}

float const* Mat44::GetAsFloatArray() const
{
	return m_values;
}

float* Mat44::GetAsFloatArray()
{
	return m_values;
}

Vec2 Mat44::GetIBasis2D() const
{
	return Vec2(m_values[Ix], m_values[Iy]);
}

Vec2 Mat44::GetJBasis2D() const
{
	return Vec2(m_values[Jx], m_values[Jy]);
}

Vec2 Mat44::GetTranslation2D() const
{
	return Vec2(m_values[Tx], m_values[Ty]);
}

Vec3 Mat44::GetIBasis3D() const
{
	return Vec3(m_values[Ix], m_values[Iy], m_values[Iz]);
}

Vec3 Mat44::GetJBasis3D() const
{
	return Vec3(m_values[Jx], m_values[Jy], m_values[Jz]); 
}

Vec3 Mat44::GetKBasis3D() const
{
	return Vec3(m_values[Kx], m_values[Ky], m_values[Kz]);
}

Vec3 Mat44::GetTranslation3D() const
{
	return Vec3(m_values[Tx], m_values[Ty], m_values[Tz]);
}

Vec4 Mat44::GetIBasis4D() const
{
	return Vec4(m_values[Ix], m_values[Iy], m_values[Iz], m_values[Iw]);
}

Vec4 Mat44::GetJBasis4D() const
{
	return Vec4(m_values[Jx], m_values[Jy], m_values[Jz], m_values[Jw]);
}

Vec4 Mat44::GetKBasis4D() const
{
	return Vec4(m_values[Kx], m_values[Ky], m_values[Kz], m_values[Kw]);
}

Vec4 Mat44::GetTranslation4D() const
{
	return Vec4(m_values[Tx], m_values[Ty], m_values[Tz], m_values[Tw]); 
}

void Mat44::SetTranslation2D(Vec2 const& translationXY)
{
	m_values[Tx] = translationXY.x;
	m_values[Ty] = translationXY.y;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;
}

void Mat44::SetTranslation3D(Vec3 const& translationXYZ)
{
	m_values[Tx] = translationXYZ.x;
	m_values[Ty] = translationXYZ.y;
	m_values[Tz] = translationXYZ.z;
	m_values[Tw] = 1.f;
}

void Mat44::SetIJ2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D)
{
	m_values[Ix] = iBasis2D.x; m_values[Iy] = iBasis2D.y; m_values[Iz] = 0.f; m_values[Iw] = 0.f;
	m_values[Jx] = jBasis2D.x; m_values[Jy] = jBasis2D.y; m_values[Jz] = 0.f; m_values[Jw] = 0.f;
}

void Mat44::SetIJT2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY)
{
	m_values[Ix] = iBasis2D.x; m_values[Iy] = iBasis2D.y; m_values[Iz] = 0.f; m_values[Iw] = 0.f;
	m_values[Jx] = jBasis2D.x; m_values[Jy] = jBasis2D.y; m_values[Jz] = 0.f; m_values[Jw] = 0.f;
	m_values[Tx] = translationXY.x; m_values[Ty] = translationXY.y; m_values[Tz] = 0.f; m_values[Tw] = 1.f;
}

void Mat44::SetIJK3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D)
{
	m_values[Ix] = iBasis3D.x; m_values[Iy] = iBasis3D.y; m_values[Iz] = iBasis3D.z; m_values[Iw] = 0.f;
	m_values[Jx] = jBasis3D.x; m_values[Jy] = jBasis3D.y; m_values[Jz] = jBasis3D.z; m_values[Jw] = 0.f;
	m_values[Kx] = kBasis3D.x; m_values[Ky] = kBasis3D.y; m_values[Kz] = kBasis3D.z; m_values[Kw] = 0.f;
}

void Mat44::SetIJKT3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ)
{
	m_values[Ix] = iBasis3D.x; m_values[Iy] = iBasis3D.y; m_values[Iz] = iBasis3D.z; m_values[Iw] = 0.f;
	m_values[Jx] = jBasis3D.x; m_values[Jy] = jBasis3D.y; m_values[Jz] = jBasis3D.z; m_values[Jw] = 0.f;
	m_values[Kx] = kBasis3D.x; m_values[Ky] = kBasis3D.y; m_values[Kz] = kBasis3D.z; m_values[Kw] = 0.f;
	m_values[Tx] = translationXYZ.x; m_values[Ty] = translationXYZ.y; m_values[Tz] = translationXYZ.z; m_values[Tw] = 1.f;
}

void Mat44::SetIJKT4D(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
{
	m_values[Ix] = iBasis4D.x; m_values[Iy] = iBasis4D.y; m_values[Iz] = iBasis4D.z; m_values[Iw] = iBasis4D.w;
	m_values[Jx] = jBasis4D.x; m_values[Jy] = jBasis4D.y; m_values[Jz] = jBasis4D.z; m_values[Jw] = jBasis4D.w;
	m_values[Kx] = kBasis4D.x; m_values[Ky] = kBasis4D.y; m_values[Kz] = kBasis4D.z; m_values[Kw] = kBasis4D.w;
	m_values[Tx] = translation4D.x; m_values[Ty] = translation4D.y; m_values[Tz] = translation4D.z; m_values[Tw] = translation4D.w;
}

void Mat44::Append(Mat44 const& matrixToAppend)
{
	float const* a = m_values;
	float const* bb = matrixToAppend.m_values;

	float c[16];

	// Column-major / basis-major multiplication:
	// Row 0: (Ix,Jx,Kx,Tx)
	// Row 1: (Iy,Jy,Ky,Ty)
	// Row 2: (Iz,Jz,Kz,Tz)
	// Row 3: (Iw,Jw,Kw,Tw)

	// Column I (index Ix..Iw)
	c[Ix] = a[Ix] * bb[Ix] + a[Jx] * bb[Iy] + a[Kx] * bb[Iz] + a[Tx] * bb[Iw];
	c[Iy] = a[Iy] * bb[Ix] + a[Jy] * bb[Iy] + a[Ky] * bb[Iz] + a[Ty] * bb[Iw];
	c[Iz] = a[Iz] * bb[Ix] + a[Jz] * bb[Iy] + a[Kz] * bb[Iz] + a[Tz] * bb[Iw];
	c[Iw] = a[Iw] * bb[Ix] + a[Jw] * bb[Iy] + a[Kw] * bb[Iz] + a[Tw] * bb[Iw];

	// Column J
	c[Jx] = a[Ix] * bb[Jx] + a[Jx] * bb[Jy] + a[Kx] * bb[Jz] + a[Tx] * bb[Jw];
	c[Jy] = a[Iy] * bb[Jx] + a[Jy] * bb[Jy] + a[Ky] * bb[Jz] + a[Ty] * bb[Jw];
	c[Jz] = a[Iz] * bb[Jx] + a[Jz] * bb[Jy] + a[Kz] * bb[Jz] + a[Tz] * bb[Jw];
	c[Jw] = a[Iw] * bb[Jx] + a[Jw] * bb[Jy] + a[Kw] * bb[Jz] + a[Tw] * bb[Jw];

	// Column K
	c[Kx] = a[Ix] * bb[Kx] + a[Jx] * bb[Ky] + a[Kx] * bb[Kz] + a[Tx] * bb[Kw];
	c[Ky] = a[Iy] * bb[Kx] + a[Jy] * bb[Ky] + a[Ky] * bb[Kz] + a[Ty] * bb[Kw];
	c[Kz] = a[Iz] * bb[Kx] + a[Jz] * bb[Ky] + a[Kz] * bb[Kz] + a[Tz] * bb[Kw];
	c[Kw] = a[Iw] * bb[Kx] + a[Jw] * bb[Ky] + a[Kw] * bb[Kz] + a[Tw] * bb[Kw];

	// Column T
	c[Tx] = a[Ix] * bb[Tx] + a[Jx] * bb[Ty] + a[Kx] * bb[Tz] + a[Tx] * bb[Tw];
	c[Ty] = a[Iy] * bb[Tx] + a[Jy] * bb[Ty] + a[Ky] * bb[Tz] + a[Ty] * bb[Tw];
	c[Tz] = a[Iz] * bb[Tx] + a[Jz] * bb[Ty] + a[Kz] * bb[Tz] + a[Tz] * bb[Tw];
	c[Tw] = a[Iw] * bb[Tx] + a[Jw] * bb[Ty] + a[Kw] * bb[Tz] + a[Tw] * bb[Tw];

	// Copy back into this
	for (int i = 0; i < 16; ++i)
	{
		m_values[i] = c[i];
	}
}

void Mat44::AppendXRotation(float degreesRotationAboutX)
{
	Mat44 rotation = MakeXRotationDegrees(degreesRotationAboutX);
	Append(rotation);
}

void Mat44::AppendYRotation(float degreesRotationAboutY)
{
	Mat44 rotation = MakeYRotationDegrees(degreesRotationAboutY);
	Append(rotation);
}

void Mat44::AppendZRotation(float degreesRotationAboutZ)
{
	Mat44 rotation = MakeZRotationDegrees(degreesRotationAboutZ);
	Append(rotation);
}

void Mat44::AppendTranslation2D(Vec2 const& translationXY)
{
	Mat44 translation = MakeTranslation2D(translationXY);
	Append(translation);
}

void Mat44::AppendTranslation3D(Vec3 const& translationXYZ)
{
	Mat44 translation = MakeTranslation3D(translationXYZ);
	Append(translation);
}

void Mat44::AppendScaleUniform2D(float uniformScaleXY)
{
	Mat44 scale = MakeUniformScale2D(uniformScaleXY);
	Append(scale);
}

void Mat44::AppendScaleUniform3D(float uniformScaleXYZ)
{
	Mat44 scale = MakeUniformScale3D(uniformScaleXYZ);
	Append(scale);
}

void Mat44::AppendScaleNonUniform2D(Vec2 const& nonUniformScaleXY)
{
	Mat44 scale = MakeNonUniformScale2D(nonUniformScaleXY);
	Append(scale);
}

void Mat44::AppendScaleNonUniform3D(Vec3 const& nonUniformScaleXYZ)
{
	Mat44 scale = MakeNonUniformScale3D(nonUniformScaleXYZ);
	Append(scale);
}

Mat44 const Mat44::MakeOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	Mat44 result;
	for (int i = 0; i < 16; ++i) result.m_values[i] = 0.0f;

	float sx = 2.0f / (right - left);
	float sy = 2.0f / (top - bottom);
	float sz = 1.0f / (zFar - zNear);

	result.m_values[Ix] = sx;
	result.m_values[Jy] = sy;
	result.m_values[Kz] = sz;

	result.m_values[Tx] = -(right + left) / (right - left);
	result.m_values[Ty] = -(top + bottom) / (top - bottom);
	result.m_values[Tz] = -zNear / (zFar - zNear);

	result.m_values[Tw] = 1.0f;

	return result;
}

Mat44 const Mat44::MakePerspectiveProjection(float fovYDegrees, float aspect, float zNear, float zFar)
{
	Mat44 result;
	for (int i = 0; i < 16; ++i) result.m_values[i] = 0.0f;

	float halfFov = 0.5f * fovYDegrees;
	float f = CosDegrees(halfFov) / SinDegrees(halfFov);
	float invD = 1.0f / (zFar - zNear);

	result.m_values[Ix] = f / aspect;
	result.m_values[Jy] = f;

	result.m_values[Kz] = zFar * invD;
	result.m_values[Kw] = 1.0f;

	result.m_values[Tz] = (-zNear * zFar) * invD;

	return result;
}

Mat44 const Mat44::GetOrthonormalInverse() const
{
	Vec3 i = GetIBasis3D();
	Vec3 j = GetJBasis3D();
	Vec3 k = GetKBasis3D();
	Vec3 t = GetTranslation3D();

	Vec3 invI(i.x, j.x, k.x);
	Vec3 invJ(i.y, j.y, k.y);
	Vec3 invK(i.z, j.z, k.z);

	Vec3 invT(
		-(invI.x * t.x + invJ.x * t.y + invK.x * t.z),
		-(invI.y * t.x + invJ.y * t.y + invK.y * t.z),
		-(invI.z * t.x + invJ.z * t.y + invK.z * t.z)
	);

	return Mat44(invI, invJ, invK, invT);
}

void Mat44::Transpose()
{
	float t;

	t = m_values[Iy]; m_values[Iy] = m_values[Jx]; m_values[Jx] = t;
	t = m_values[Iz]; m_values[Iz] = m_values[Kx]; m_values[Kx] = t;
	t = m_values[Iw]; m_values[Iw] = m_values[Tx]; m_values[Tx] = t;

	t = m_values[Jz]; m_values[Jz] = m_values[Ky]; m_values[Ky] = t;
	t = m_values[Jw]; m_values[Jw] = m_values[Ty]; m_values[Ty] = t;

	t = m_values[Kw]; m_values[Kw] = m_values[Tz]; m_values[Tz] = t;
}

void Mat44::Orthonormalize_XFwd_YLeft_ZUp2()
{
	Vec3 i = GetIBasis3D();
	Vec3 j = GetJBasis3D();

	i = i.GetNormalized();

	j = j - DotProduct3D(j, i) * i;
	j = j.GetNormalized();

	Vec3 k = CrossProduct3D(i, j).GetNormalized();

	j = CrossProduct3D(k, i).GetNormalized();

	Vec3 t = GetTranslation3D();
	SetIJKT3D(i, j, k, t);
}

