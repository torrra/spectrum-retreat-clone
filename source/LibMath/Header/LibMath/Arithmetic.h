#ifndef __LIBMATH__ARITHMETIC_H__
#define __LIBMATH__ARITHMETIC_H__

namespace LibMath
{
	bool			almostEqual(float, float);		// Return if two floating value are similar enought to be considered equal

	float			ceiling(float);				// Return lowest integer value higher or equal to parameter
	float			clamp(float, float, float);	// Return parameter limited by the given range
	float			floor(float);					// Return highest integer value lower or equal to parameter
	float			squareRoot(float);			// Return square root of parameter
	float			wrap(float, float, float);	// Return parameter as value inside the given range
	float			power(float, unsigned int);

	unsigned int	factorial(unsigned int);

	float			absolute(float);
	int				absolute(int);

	float			min(float, float);
	float			max(float, float);
}

namespace lm = LibMath;

#endif // !__LIBMATH__ARITHMETIC_H__
