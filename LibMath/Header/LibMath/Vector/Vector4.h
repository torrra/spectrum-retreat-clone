#ifndef __LIBMATH__VECTOR__VECTOR4_H__
#define __LIBMATH__VECTOR__VECTOR4_H__

#include "Vector3.h"
#include "Vector2.h"

namespace LibMath
{
	// 4D vector intended to store a 3D vector's homogenous coordinates
	class Vector4
	{
	public:

		Vector4() = default;

		Vector4(const Vector4& other);
		Vector4(const Vector3& other);
		Vector4(const Vector3& other, float w);

		Vector4(float x, float y, float z, float w);

		Vector4(const Vector2& first, const Vector2& second);

		~Vector4() = default;

		Vector3		to3D(void);

		operator	Vector3() const;

		Vector4&	operator=(const Vector4& rhs);
		Vector4&	operator=(const Vector3& rhs);
		float&		operator[](int index);
		float		operator[](int index) const;

		Vector4&	operator/=(float divisor);
		Vector4		operator*(float rhs);


		float m_x = 0.f;
		float m_y = 0.f;
		float m_z = 0.f;
		float m_w = 1.f;
	};
}

#ifdef __LIBMATH__MATRIX__MATRIX4_H__
#include "../Matrix4Vector4Operation.h"
#endif // __LIBMATH__MATRIX__MATRIX4_H__

#endif // !__LIBMATH__VECTOR__VECTOR4_H__
