#include <cmath>

#include "Trigonometry.h"

namespace LibMath
{
	const  float g_pi = 3.14159265359F;

	float	cos(Radian angle)
	{
		return cosf(angle.radian(false));
	}

	float	sin(Radian angle)
	{
		return sinf(angle.radian(false));
	}

	float	tan(Radian angle)
	{
		return tanf(angle.radian(false));
	}

	Radian	asin(float sine)
	{
		return Radian(asinf(sine));
	}

	Radian	acos(float cosine)
	{
		return Radian(acosf(cosine));
	}

	Radian  atan(float tangent)
	{
		return Radian(atanf(tangent));
	}

}