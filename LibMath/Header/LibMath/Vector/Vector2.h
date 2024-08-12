#ifndef __LIBMATH__VECTOR__VECTOR2_H__
#define __LIBMATH__VECTOR__VECTOR2_H__

#include <string>

#include "../Angle/Radian.h"

namespace LibMath
{
	class Vector3;

	class Vector2
	{
	public:
						Vector2();
		explicit		Vector2(float);
						Vector2(float, float);
						Vector2(const Vector2&);
						~Vector2();


		Vector2&		operator=(const Vector2&);

		float&			operator[](int);
		float			operator[](int) const;

		Radian			angleFrom(const Vector2&);

		float			dot(const Vector2&);
		float			cross(const Vector2&);

		float			magnitude();
		float			magnitudeSquared();

		void			normalize(void);
		void			projectOnto(Vector2 const&);					// project this vector onto an other

		void			reflectOnto(Vector2 const&);					// reflect this vector by an other

		void			rotate(Radian);									// rotate this vector using euler angle apply in the z, x, y order

		void			scale(Vector2 const&);							// scale this vector by a given factor

		std::string		string() const;									// return a string representation of this vector
		std::string		stringLong() const;								// return a verbose string representation of this vector

		void			translate(Vector2 const&);						// offset this vector by a given distance

		Vector3			to3D(void);

		float			m_x = 0.F;
		float			m_y = 0.F;

	};
}

#endif // !__LIBMATH__VECTOR__VECTOR2_H__