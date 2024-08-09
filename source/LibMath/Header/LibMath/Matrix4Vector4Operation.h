#ifndef __LIBMATH__MATRIX4VECTOR4OPERATION_H_
#define __LIBMATH__MATRIX4VECTOR4OPERATION_H_

#include "Vector/Vector4.h"
#include "Matrix/Matrix4.h"

namespace LibMath
{
	Vector4 operator*(Matrix4 const&, Vector4 const&);
}

namespace lm = LibMath;

#endif // !__LIBMATH__MATRIX4VECTOR4OPERATION_H_
