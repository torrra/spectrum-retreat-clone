#include <iostream>
#include <sstream>
#include <string>

#include "Vector.h"
#include "Arithmetic.h"
#include "Trigonometry.h"

namespace LibMath
{

		Vector3::Vector3() {}
		// set all component to 0

		Vector3::Vector3(float val)
		: m_x(val), m_y(val), m_z(val) {}									// set all component to the same value

		Vector3::Vector3(float x, float y, float z)
		: m_x(x), m_y(y), m_z(z) {}					// set all component individually

		Vector3::Vector3(Vector3 const& other)
		: m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {}						// copy all component

		Vector3::~Vector3() {}

		Vector3			Vector3::zero()
		{
			return Vector3(0.F, 0.F, 0.F);
		}											// return a vector with all its component set to 0

		Vector3			Vector3::one()
		{
			return Vector3(1.F, 1.F, 1.F);
		}	// return a vector with all its component set to 1

		Vector3			Vector3::up()
		{
			return Vector3(0.F, 1.F, 0.F);
		}	// return a unit vector pointing upward

		Vector3			Vector3::down()
		{
			return Vector3(0.F, -1.F, 0.F);
		}	// return a unit vector pointing downward

		Vector3			Vector3::left()
		{
			return Vector3(-1.F, 0.F, 0.F);
		}	// return a unit vector pointing left

		Vector3			Vector3::right()
		{
			return Vector3(1.F, 0.F, 0.F);
		}	// return a unit vector pointing right

		Vector3			Vector3::front()
		{
			return Vector3(0.F, 0.F, 1.F);
		}	// return a unit vector pointing forward

		Vector3			Vector3::back()
		{
			return Vector3(0.F, 0.F, -1.F);
		}	// return a unit vector pointing backward

		Vector3&		Vector3::operator=(Vector3 const& rhs)
		{
			m_x = rhs.m_x;
			m_y = rhs.m_y;
			m_z = rhs.m_z;

			return *this;
		}

		float&			Vector3::operator[](int index)
						{
							switch(index)
							{
							case 0:
								return m_x;
							case 1:
								return m_y;
							default:
								return m_z;
							}
						}
								// return this vector component value

		float			Vector3::operator[](int index) const
		{
			switch (index)
			{
			case 0:
				return m_x;
			case 1:
				return m_y;
			case 2:
				return m_z;
			default:
				return 0.F;
			}
		}// return this vector component value

		Radian			Vector3::angleFrom(Vector3 const& other) const
		{
			float		cosAngle = dot(other) / (magnitude() * other.magnitude());

			return Radian(acos(cosAngle));
		}// return smallest angle between 2 vector

		Vector3			Vector3::cross(Vector3 const& other) const
		{
			float		prodX = (m_y * other.m_z) - (m_z * other.m_y),
						prodY = (m_z * other.m_x) - (m_x * other.m_z),
						prodZ = (m_x * other.m_y) - (m_y * other.m_x);

			return Vector3(prodX, prodY, prodZ);
		}// return a copy of the cross product result

		float			Vector3::distanceFrom(Vector3 const& other) const
		{
			float		diffX = other.m_x - m_x,
						diffY = other.m_y - m_y,
						diffZ = other.m_z - m_z;

			Vector3		distance(diffX, diffY, diffZ);

			return distance.magnitude();
		}// return distance between 2 points

		float			Vector3::distanceSquaredFrom(Vector3 const& other) const
		{
			float		distance = distanceFrom(other);

			return distance * distance;
		}// return square value of the distance between 2 points


		float			Vector3::distance2DFrom(Vector3 const& other) const
		{
			float		diffX = other.m_x - m_x,
						diffY = other.m_y - m_y;


			Vector2		distance(diffX, diffY);

			return distance.magnitude();
		}			// return the distance between 2 points on the X-Y axis only

		float			Vector3::distance2DSquaredFrom(Vector3 const& other) const
		{
			float		distance = distance2DFrom(other);

			return distance * distance;
		}// return the square value of the distance between 2 points points on the X-Y axis only

		float			Vector3::dot(Vector3 const& other) const
		{
			return (m_x * other.m_x) + (m_y * other.m_y) + (m_z * other.m_z);
		}// return dot product result

		bool			Vector3::isLongerThan(Vector3 const& other) const { return magnitude() > other.magnitude(); }		// return true if this vector magnitude is greater than the other
		bool			Vector3::isShorterThan(Vector3 const& other) const { return magnitude() < other.magnitude(); }			// return true if this vector magnitude is less than the other

		bool			Vector3::isUnitVector() const { return magnitude() == 1.F; }							// return true if this vector magnitude is 1

		float			Vector3::magnitude() const
		{
			return squareRoot((m_x * m_x) + (m_y * m_y) + (m_z * m_z));
		}// return vector magnitude
		float			Vector3::magnitudeSquared() const { float mag = magnitude(); return mag * mag; }						// return square value of the vector magnitude

		void			Vector3::normalize()
		{
			float	mag = magnitude();

			m_x /= mag;
			m_y /= mag;
			m_z /= mag;
		}// scale this vector to have a magnitude of 1

		Vector3			Vector3::normalizedCopy() const
		{
			float		mag = magnitude();
			Vector3		normalized = *this;

			normalized.m_x /= mag;
			normalized.m_y /= mag;
			normalized.m_z /= mag;

			return normalized;
		}// scale this vector to have a magnitude of 1

		void			Vector3::projectOnto(Vector3 const& dest)
		{
			float		projTerm = dot(dest) / dest.magnitudeSquared();

			m_x = projTerm * dest.m_x;
			m_y = projTerm * dest.m_y;
			m_z = projTerm * dest.m_z;
		}// project this vector onto an other

		void			Vector3::reflectOnto(Vector3 const& axis)
		{
			Vector3		normalizedAxis = axis.normalizedCopy();

			*this -= (this->dot(normalizedAxis) * 2.f) * normalizedAxis;


		}// reflect this vector by an other

		void			Vector3::rotate(Radian xAngle, Radian zAngle, Radian yAngle)
		{

			if (zAngle.radian() != 0.f)
			{
				const float xCpy = m_x, yCpy = m_y;

				float	cosineZ = cos(zAngle), sineZ = sin(zAngle);

				m_x = (cosineZ * xCpy) - (sineZ * yCpy);
				m_y = (sineZ * xCpy) + (cosineZ * yCpy);
			}

			if (xAngle.radian() != 0.f)
			{
				const float yCpy = m_y, zCpy = m_z;

				float	cosineX = cos(xAngle), sineX = sin(xAngle);

				m_y = (cosineX * yCpy) - (sineX * zCpy);
				m_z = (sineX * yCpy) + (cosineX * zCpy);
			}

			if (yAngle.radian() != 0.f)
			{
				const float xCpy = m_x, zCpy = m_z;

				float	cosineY = cos(yAngle), sineY = sin(yAngle);

				m_x = (cosineY * xCpy) + (sineY * zCpy);
				m_z = ((-sineY) * xCpy) + (cosineY * zCpy);
			}


		}// rotate this vector using euler angle apply in the z, x, y order


		void			Vector3::rotate(Radian angle, Vector3 const& axis)					// rotate this vector around an arbitrary axis
		{
			const Vector3		normalizedAxis = axis.normalizedCopy();
			const float			xCpy = m_x, yCpy = m_y, zCpy = m_z;
			const float			cosAngle = cos(angle), sinAngle = sin(angle);
			const float			oneMinCos = 1.f - cosAngle;

			float	factorX = cosAngle + normalizedAxis.m_x * normalizedAxis.m_x * oneMinCos;
			float	factorY = normalizedAxis.m_x * normalizedAxis.m_y * oneMinCos - normalizedAxis.m_z * sinAngle;
			float	factorZ = normalizedAxis.m_x * normalizedAxis.m_z * oneMinCos + normalizedAxis.m_y * sinAngle;

			m_x = xCpy * factorX + yCpy * factorY + factorZ * zCpy;

			factorX = normalizedAxis.m_y * normalizedAxis.m_x * oneMinCos + normalizedAxis.m_z * sinAngle;
			factorY = cosAngle + normalizedAxis.m_y * normalizedAxis.m_y * oneMinCos;
			factorZ = normalizedAxis.m_y * normalizedAxis.m_z * oneMinCos - normalizedAxis.m_x * sinAngle;

			m_y = xCpy * factorX + yCpy * factorY + factorZ * zCpy;

			factorX = normalizedAxis.m_z * normalizedAxis.m_x * oneMinCos - normalizedAxis.m_y * sinAngle;
			factorY = normalizedAxis.m_z * normalizedAxis.m_y * oneMinCos + normalizedAxis.m_x * sinAngle;
			factorZ = cosAngle + normalizedAxis.m_z * normalizedAxis.m_z * oneMinCos;

			m_z = xCpy * factorX + yCpy * factorY + factorZ * zCpy;

		}
			//void			rotate(Quaternion const&); todo quaternion		// rotate this vector using a quaternion rotor

		void			Vector3::scale(Vector3 const& other)
		{
			*this *= other;
		}// scale this vector by a given factor

		std::string		Vector3::string() const
		{
			std::stringstream vectorString;

			vectorString << '{' << m_x << ',' << m_y << ',' << m_z << '}';

			return vectorString.str();


		}// return a string representation of this vector

		std::string		Vector3::stringLong() const
		{
			std::stringstream vectorString;

			vectorString << "Vector3{ x:" << m_x << ", y:" << m_y << ", z:" << m_z << " }";

			return vectorString.str();
		}// return a verbose string representation of this vector

		void			Vector3::translate(Vector3 const& other)
		{
			*this += other;
		}// offset this vector by a given distance

		Vector2			Vector3::to2D(void)
		{
			return Vector2(m_x, m_y);
		}


	bool			operator==(Vector3 const& lhs, Vector3 const& rhs)
	{
		return (almostEqual(lhs.m_x, rhs.m_x) && almostEqual(lhs.m_y, rhs.m_y) && almostEqual(lhs.m_z, rhs.m_z)) ? true : false;
	}// Vector3{ 1 } == Vector3::one()				// true					// return if 2 vectors have the same component


	bool			operator!=(Vector3 const& lhs, Vector3 const& rhs)
	{
		return !(lhs == rhs);
	}// Vector3{ 1 } != Vector3::one()				// false				// return if 2 vectors differ by at least a component

	Vector3			operator-(Vector3 rhs)
	{
		return Vector3(-rhs.m_x, -rhs.m_y, -rhs.m_z);
	}// - Vector3{ .5, 1.5, -2.5 }					// { -.5, -1.5, 2.5 }	// return a copy of a vector with all its component inverted

	Vector3			operator+(Vector3 lhs, Vector3 const& rhs)
	{
		return Vector3(lhs.m_x + rhs.m_x, lhs.m_y + rhs.m_y, lhs.m_z + rhs.m_z);
	}// Vector3{ .5, 1.5, -2.5 } + Vector3::one()	// { 1.5, 2.5, -1.5 }	// add 2 vectors component wise


	//Vector3			operator-(Vector3 lhs, Vector3 const& rhs)
	//{
	//	return Vector3(lhs.m_x - rhs.m_x, lhs.m_y - rhs.m_y, lhs.m_z - rhs.m_z);
	//} // Vector3{ .5, 1.5, -2.5 } - Vector3{ 1 }		// { -.5, .5, -3.5 }	// substract 2 vectors component wise

	Vector3			operator-(Vector3 const& lhs, Vector3 const& rhs)
	{
		return Vector3(lhs.m_x - rhs.m_x, lhs.m_y - rhs.m_y, lhs.m_z - rhs.m_z);
	}


	Vector3			operator*(Vector3 lhs, Vector3 const& rhs)
	{
		return Vector3(lhs.m_x * rhs.m_x, lhs.m_y * rhs.m_y, lhs.m_z * rhs.m_z);
	}// Vector3{ .5, 1.5, -2.5 } * Vector3::zero()	// { 0, 0, 0 }			// multiply 2 vectors component wise


	Vector3			operator/(Vector3 lhs, Vector3 const& rhs)
	{
		return Vector3(lhs.m_x / rhs.m_x, lhs.m_y / rhs.m_y, lhs.m_z / rhs.m_z);
	}// Vector3{ .5, 1.5, -2.5 } / Vector3{ 2 }		// { .25, .75, -1.25 }	// divide 2 vectors component wise

	Vector3			operator*(Vector3 lhs, float rhs)
	{
		return Vector3(lhs.m_x * rhs, lhs.m_y * rhs, lhs.m_z * rhs);
	}

	Vector3			operator*(float lhs, Vector3 rhs)
	{
		return rhs * lhs;
	}


	Vector3& operator+=(Vector3& lhs, Vector3 const& rhs)
	{
		lhs.m_x += rhs.m_x;
		lhs.m_y += rhs.m_y;
		lhs.m_z += rhs.m_z;

		return lhs;
	}// addition component wise


	Vector3& operator-=(Vector3& lhs, Vector3 const& rhs)
	{
		lhs.m_x -= rhs.m_x;
		lhs.m_y -= rhs.m_y;
		lhs.m_z -= rhs.m_z;

		return lhs;
	}// substraction component wise


	Vector3& operator*=(Vector3& lhs, Vector3 const& rhs)
	{
		lhs.m_x *= rhs.m_x;
		lhs.m_y *= rhs.m_y;
		lhs.m_z *= rhs.m_z;

		return lhs;
	}// multiplication component wise


	Vector3& operator/=(Vector3& lhs, Vector3 const& rhs)
	{
		lhs.m_x /= rhs.m_x;
		lhs.m_y /= rhs.m_y;
		lhs.m_z /= rhs.m_z;

		return lhs;
	}// division component wise

	std::ostream& operator<<(std::ostream& os, Vector3 const& rhs)
	{
		os << rhs.string();

		return os;
	}
	std::istream& operator>>(std::istream& is, Vector3& rhs)
	{
		char discard;

		is >> discard >> rhs.m_x >> discard >> rhs.m_y >> discard >> rhs.m_z >> discard;
		return is;
	}
	// cout << Vector3{ .5, 1.5, -2.5 }				// add a vector string representation to an output stream



	Vector2::Vector2() {}

	Vector2::Vector2(float val)
	: m_x(val), m_y(val) {}


	Vector2::Vector2(float x, float y)
	: m_x(x), m_y(y) {}

	Vector2::Vector2(const Vector2& other)
	: m_x(other.m_x), m_y(other.m_y) {}

	Vector2::~Vector2(){}


	Vector2& Vector2::operator=(const Vector2& rhs)
	{
		m_x = rhs.m_x;
		m_y = rhs.m_y;

		return *this;
	}


	float			Vector2::magnitude()
	{
		return squareRoot((m_x * m_x) + (m_y * m_y));
	}

	LibMath::Vector4::Vector4(const Vector4& other)
		: m_x(other.m_x), m_y(other.m_y), m_z(other.m_z), m_w(other.m_w)
	{}

	LibMath::Vector4::Vector4(const Vector3& other)
		: m_x(other.m_x), m_y(other.m_y), m_z(other.m_z)
	{}

	LibMath::Vector4::Vector4(const Vector3& other, float w)
		: m_x(other.m_x), m_y(other.m_y), m_z(other.m_z), m_w(w)
	{}

	LibMath::Vector4::Vector4(float x, float y, float z, float w)
		: m_x(x), m_y(y), m_z(z), m_w(w)
	{}

	LibMath::Vector4::Vector4(const Vector2& first, const Vector2& second)
		: m_x(first.m_x), m_y(first.m_y), m_z(second.m_x), m_w(second.m_y)
	{}

	LibMath::Vector4::operator Vector3() const
	{
		return Vector3(m_x, m_y, m_z);
	}

	Vector4& LibMath::Vector4::operator=(const Vector4& rhs)
	{
		m_x = rhs.m_x;
		m_y = rhs.m_y;
		m_z = rhs.m_z;
		m_w = rhs.m_w;

		return *this;
	}


	Vector4& LibMath::Vector4::operator=(const Vector3& rhs)
	{
		m_x = rhs.m_x;
		m_y = rhs.m_y;
		m_z = rhs.m_z;
		m_w = 1.f;

		return *this;
	}

	float& LibMath::Vector4::operator[](int index)
	{
		switch (index)
		{
		case 0:
			return m_x;
		case 1:
			return m_y;
		case 2:
			return m_z;
		default:
			return m_w;
		}
	}

	float LibMath::Vector4::operator[](int index) const
	{
		switch (index)
		{
		case 0:
			return m_x;
		case 1:
			return m_y;
		case 2:
			return m_z;
		case 3:
			return m_w;
		default:
			return 0.F;
		}
	}

	Vector3 LibMath::Vector4::to3D(void)
	{
		return Vector3(m_x, m_y, m_z);
	}

	Vector4& LibMath::Vector4::operator/=(float divisor)
	{
		m_x /= divisor;
		m_y /= divisor;
		m_z /= divisor;
		m_w /= divisor;

		return *this;
	}

	Vector4 LibMath::Vector4::operator*(float rhs)
	{
		return Vector4(m_x * rhs, m_y * rhs, m_z * rhs, m_w * rhs);
	}

}