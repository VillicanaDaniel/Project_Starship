#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"

//------------------------------------------------------------------------------------------------
// 4x4 Homogeneous 2D/3D transformation matrix, stored basis-major in memory (Ix,Iy,Iz,Iw,Jx,...).
//
// Note: we specifically do NOT provide an operator* overload; since doing so would require a
//       decision on whether to write it to work correctly with EITHER column-major or row-major
//       style matrix notation.  They correspond to different ways of writing operator*, and in
//       order to implement an operator*, we are forced to make a notational commitment.  This is
//       certainly ambiguous to the reader, and at the very least, probably very confusing.  Instead,
//       we prefer to use method names, such as “Append”, which are more neutral (e.g. multiply a
//       new matrix “on the right” in column-major notation OR “on the left” in row-major notation).
//------------------------------------------------------------------------------------------------
struct Mat44
{
public:
	enum eIndex
	{
		Ix, Iy, Iz, Iw,
		Jx, Jy, Jz, Jw,
		Kx, Ky, Kz, Kw,
		Tx, Ty, Tz, Tw
	};

	float m_values[16];     // stored in “basis major” order (Ix,Iy,Iz,Iw,Jx…)

	Mat44();                                            // Default constructor = IDENTITY matrix!
	explicit Mat44(float const* sixteenBasisMajor);
	explicit Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D);
	explicit Mat44(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D);
	explicit Mat44(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D);

	static Mat44 MakeTranslation2D(Vec2 const& translationXY);
	static Mat44 MakeTranslation3D(Vec3 const& translationXYZ);
	static Mat44 MakeUniformScale2D(float uniformScaleXY);
	static Mat44 MakeUniformScale3D(float uniformScaleXYZ);
	static Mat44 MakeNonUniformScale2D(Vec2 const& nonUniformScaleXY);
	static Mat44 MakeNonUniformScale3D(Vec3 const& nonUniformScaleXYZ);
	static Mat44 MakeZRotationDegrees(float rotationDegreesAboutZ);
	static Mat44 MakeYRotationDegrees(float rotationDegreesAboutY);
	static Mat44 MakeXRotationDegrees(float rotationDegreesAboutX);


	Vec2 TransformVectorQuantity2D(Vec2 const& vectorQuantityXY) const;  // assumes z=0, w=0
	Vec3 TransformVectorQuantity3D(Vec3 const& vectorQuantityXYZ) const; // assumes w=0
	Vec2 TransformPosition2D(Vec2 const& positionXY) const;              // assumes z=0, w=1
	Vec3 TransformPosition3D(Vec3 const& positionXYZ) const;             // assumes w=1
	Vec4 TransformHomogeneous3D(Vec4 const& homogeneousPoint3D) const; // w is provided

	float const* GetAsFloatArray() const;     // non-const (mutable) version
	float* GetAsFloatArray();           // const version, used only when Mat44 is const

	Vec2 GetIBasis2D() const;
	Vec2 GetJBasis2D() const;
	Vec2 GetTranslation2D() const;

	Vec3 GetIBasis3D() const;
	Vec3 GetJBasis3D() const;
	Vec3 GetKBasis3D() const;
	Vec3 GetTranslation3D() const;

	Vec4 GetIBasis4D() const;
	Vec4 GetJBasis4D() const;
	Vec4 GetKBasis4D() const;
	Vec4 GetTranslation4D() const;

	void SetTranslation2D(Vec2 const& translationXY);                     // Sets translation.z = 0, translation.w = 1
	void SetTranslation3D(Vec3 const& translationXYZ);                    // Sets translation.w = 1

	void SetIJ2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D);             // Sets z=0,w=0 for i,j; does not modify tx or ty
	void SetIJT2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY);
	void SetIJK3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D); // Sets w=0 for i,j,k; tx,ty,tz remain unchanged
	void SetIJKT3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ);
	void SetIJKT4D(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D); // All 16 values provided

	void Append(Mat44 const& matrixToAppend);        // multiply on right in column notation (“me = oldMe * M”)
	void AppendXRotation(float degreesRotationAboutX);
	void AppendYRotation(float degreesRotationAboutY);
	void AppendZRotation(float degreesRotationAboutZ);
	void AppendTranslation2D(Vec2 const& translationXY);
	void AppendTranslation3D(Vec3 const& translationXYZ);
	void AppendScaleUniform2D(float uniformScaleXY);     // translation should remain unaffected
	void AppendScaleUniform3D(float uniformScaleXYZ);    // translation should remain unaffected
	void AppendScaleNonUniform2D(Vec2 const& nonUniformScaleXY); // x and y bases should remain orthogonal
	void AppendScaleNonUniform3D(Vec3 const& nonUniformScaleXYZ); // z and t bases should remain unaffected

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	static Mat44 const MakeOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar);
	static Mat44 const MakePerspectiveProjection(float fovYDegrees, float aspect, float zNear, float zFar);
	Mat44 const GetOrthonormalInverse() const;
	void Transpose();
	void Orthonormalize_XFwd_YLeft_ZUp2();
};