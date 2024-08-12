#ifndef __LIBMATH__ANGLE__DEGREE_H__
#define __LIBMATH__ANGLE__DEGREE_H__

namespace LibMath
{
	class Radian;

	class Degree
	{
	public:
		Degree();
		explicit	Degree(float);				// explicit so no ambiguous / implicit conversion from float to angle can happen
		Degree(Degree const&);
		~Degree();

		operator Radian() const;				// Radian angle = Degree{45};		// implicit conversion from Degree to Radian

		Degree& operator=(Degree);
		Degree& operator+=(Degree);			// Degree angle += Degree{45};
		Degree& operator-=(Degree);			// Degree angle -= Degree{45};
		Degree& operator*=(float);			// Degree angle *= 3;
		Degree& operator/=(float);			// Degree angle /= 3;

		void		wrap(bool = false);			// true -> limite m_value to range [-180, 180[	// false -> limite m_value to range [0, 360[

		float		degree(bool = false) const;	// return angle in degree	// true -> limite value to range [-180, 180[	// false -> limite value to range [0, 360[
		float		radian(bool = true) const;	// return angle in radian	// true -> limite value to range [-pi, pi[		// false -> limite value to range [0, 2 pi[
		float		raw() const;				// return m_angle

	private:
		float m_value = 0.0F;
	};

	bool	operator==(Degree, Degree);			// bool isEqual = Degree{45} == Degree{45};		// true
	bool	operator==(Degree, Radian const&);	// bool isEqual = Degree{60} == Radian{0.5};	// false

	Degree	operator-(Degree);					// Degree angle = - Degree{45};					// Degree{-45}

	Degree	operator+(Degree, Degree);			// Degree angle = Degree{45} + Degree{45};		// Degree{90}
	Degree	operator-(Degree, Degree);			// Degree angle = Degree{45} - Degree{45};		// Degree{0}
	Degree	operator*(Degree, float);			// Degree angle = Degree{45} * 3;				// Degree{135}
	Degree	operator/(Degree, float);			// Degree angle = Degree{45} / 3;				// Degree{15}

	inline namespace Literal
	{
		LibMath::Degree operator""_deg(long double);			// Degree angle = 45_deg;
		LibMath::Degree operator""_deg(unsigned long long int);	// Degree angle = 7.5_deg;
	}
}

#endif // !__LIBMATH__ANGLE__DEGREE_H__
