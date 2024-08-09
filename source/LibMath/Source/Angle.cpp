#include "Angle.h"
#include "Arithmetic.h"

#define PI 3.14159265359F
#define RAD2DEG 180.0F / PI
#define DEG2RAD PI / 180.0F

#define DEG_PRECISION 1.0F
#define RAD_PRECISION 0.001F

namespace LibMath
{
	Degree::Degree() = default;
	Degree::Degree(float val) : m_value(val) {}				// explicit so no ambiguous / implicit conversion from float to angle can happen
	Degree::Degree(Degree const& other) : m_value(other.m_value) {}
	Degree::~Degree() = default;

	Degree::operator Radian() const
	{
		float radVal = m_value * DEG2RAD;
		return Radian(radVal);
	}				// Radian angle = Degree{45};		// implicit conversion from Degree to Radian

	Degree& Degree::operator=(Degree rhs) { this->m_value = rhs.m_value; return *this; }


	Degree& Degree::operator+=(Degree rhs) { this->m_value += rhs.m_value; return *this; }			// Degree angle += Degree{45};
	Degree& Degree::operator-=(Degree rhs) { this->m_value -= rhs.m_value; return *this; }			// Degree angle -= Degree{45};


	Degree& Degree::operator*=(float factor) { this->m_value *= factor; return *this; }			// Degree angle *= 3;
	Degree& Degree::operator/=(float divider) { this->m_value /= divider; return *this; }			// Degree angle /= 3;

	void		Degree::wrap(bool limit) { m_value = degree(limit); }			// true -> limite m_value to range [-180, 180[	// false -> limite m_value to range [0, 360[

	float		Degree::degree(bool limit) const
	{
		const float		full_circle = 360.0F, half_circle = 180.0F;
		float			abs_value = (m_value >= 0) ? m_value : -m_value;

		if (abs_value >= full_circle)
		{
			int		circle_count = static_cast<int>(abs_value / full_circle);

			abs_value -= circle_count * full_circle;
		}

		if (limit)
		{
			if (abs_value >= half_circle)
				abs_value -= full_circle;

			if (m_value < 0)
				abs_value = -abs_value;
		}
		else if (!limit && m_value < 0)
		{
			abs_value = full_circle - abs_value;
		}

		return abs_value;
	}
	// return angle in degree	// true -> limite value to range [-180, 180[	// false -> limite value to range [0, 360[


	float		Degree::radian(bool limit) const
	{

		const float		full_circle = 360.F, rad_circle = PI * 2.F;
		float			abs_value = (m_value >= 0) ? m_value : -m_value;

		if (abs_value >= full_circle)
		{
			int		circle_count = static_cast<int>(abs_value / full_circle);

			abs_value -= circle_count * full_circle;
		}

		abs_value *= DEG2RAD;

		if (limit)
		{
			if (abs_value >= PI)
				abs_value -= rad_circle;

			if (m_value < 0)
				abs_value = -abs_value;
		}
		else if (!limit && m_value < 0)
		{
			abs_value = rad_circle - abs_value;
		}

		return abs_value;


	}// return angle in radian	// true -> limite value to range [-pi, pi[		// false -> limite value to range [0, 2 pi[
	float		Degree::raw() const { return m_value; }				// return m_angle



	bool	operator==(Degree lhs, Degree rhs) { return almostEqual(lhs.degree(), rhs.degree()); } 			// bool isEqual = Degree{45} == Degree{45};		// true
	bool	operator==(Degree lhs, Radian const& rhs) { return almostEqual(lhs.degree(), rhs.degree()); }	// bool isEqual = Degree{60} == Radian{0.5};	// false

	Degree	operator-(Degree rhs) { return Degree(-rhs.degree()); }					// Degree angle = - Degree{45};					// Degree{-45}

	Degree	operator+(Degree lhs, Degree rhs) { float sum_val = lhs.degree() + rhs.degree(); return Degree(sum_val); }			// Degree angle = Degree{45} + Degree{45};		// Degree{90}
	Degree	operator-(Degree lhs, Degree rhs) { return lhs + (-rhs); }			// Degree angle = Degree{45} - Degree{45};		// Degree{0}
	Degree	operator*(Degree lhs, float factor) { float product_val = lhs.degree() * factor; return Degree(product_val); }			// Degree angle = Degree{45} * 3;				// Degree{135}
	Degree	operator/(Degree lhs, float diviser) { float product_val = lhs.degree() / diviser; return Degree(product_val); }				// Degree angle = Degree{45} / 3;				// Degree{15}

	inline namespace Literal
	{
		LibMath::Degree operator""_deg(long double val) { return Degree(static_cast<float>(val)); }			// Degree angle = 45_deg;
		LibMath::Degree operator""_deg(unsigned long long int val) { return Degree(static_cast<float>(val)); }	// Degree angle = 7.5_deg;
	}

	Radian::Radian() = default;
	Radian::Radian(float val) : m_value(val) {}				// explicit so no ambiguous / implicit conversion from float to angle can happen
	Radian::Radian(Radian const& other) : m_value(other.m_value) {}
	Radian::~Radian() = default;

	Radian::operator Degree() const
	{
		//float deg_val = m_value * RAD2DEG;
		return Degree(m_value * RAD2DEG);
	}	// Degree angle = Radian{0.5};		// implicit conversion from Radian to Degree

	Radian& Radian::operator=(Radian rhs) { this->m_value = rhs.m_value; return *this; }
	Radian& Radian::operator+=(Radian rhs) { this->m_value += rhs.m_value; return *this; }		// Radian angle += Radian{0.5};
	Radian& Radian::operator-=(Radian rhs) { this->m_value -= rhs.m_value; return *this; }			// Radian angle -= Radian{0.5};
	Radian& Radian::operator*=(float factor) { this->m_value *= factor; return *this; }		// Radian angle *= 3;
	Radian& Radian::operator/=(float divider) { this->m_value /= divider; return *this; }			// Radian angle /= 3;

	void		Radian::wrap(bool limit)
	{
		m_value = radian(limit);
	}
	// true -> limite m_value to range [-pi, pi[	// false -> limite m_value to range [0, 2 pi[

	float		Radian::degree(bool limit) const
	{
		const float		full_circle = PI * 2.F, deg_circle = 360.F, half_circle = 180.F;
		float			abs_value = (m_value >= 0) ? m_value : -m_value;

		if (abs_value >= full_circle)
		{
			int		circle_count = static_cast<int>(abs_value / full_circle);

			abs_value -= circle_count * full_circle;
		}

		abs_value *= RAD2DEG;

		if (limit)
		{
			if (abs_value >= half_circle)
				abs_value -= deg_circle;

			if (m_value < 0 && abs_value > 0)
				abs_value = -abs_value;
		}
		else if (!limit && m_value < 0)
		{
			abs_value = deg_circle - abs_value;
		}

		return abs_value;
	}	// return angle in degree	// true -> limite value to range [-180, 180[	// false -> limite value to range [0, 360[


	float		Radian::radian(bool limit) const
	{
		const float		full_circle = PI * 2.F;
		float			abs_value = (m_value >= 0) ? m_value : -m_value;

		if (abs_value >= full_circle)
		{
			int		circle_count = static_cast<int>(abs_value / full_circle);

			abs_value -= circle_count * full_circle;
		}

		if (limit)
		{
			if (abs_value >= PI)
				abs_value -= full_circle;

			if (m_value < 0 && abs_value > 0)
				abs_value = -abs_value;
		}
		else if (!limit && m_value < 0)
		{
			abs_value = full_circle - abs_value;
		}

		return abs_value;

	}	// return angle in radian	// true -> limite value to range [-pi, pi[		// false -> limite value to range [0, 2 pi[
	float		Radian::raw() const { return m_value; }				// return m_angle

	bool	operator==(Radian lhs, Radian rhs) { return lhs.radian() - rhs.radian() < RAD_PRECISION; }	// bool isEqual = Radian{0.5} == Radian{0.5};	// true
	bool	operator==(Radian lhs, Degree const& rhs) { return lhs.radian() - rhs.radian() < RAD_PRECISION; }	// bool isEqual = Radian{0.5} == Degree{60};	// false

	Radian	operator-(Radian rhs) { return Radian(-rhs.radian()); }					// Degree angle = - Radian{0.5};				// Radian{-0.5}

	Radian	operator+(Radian lhs, Radian rhs)
	{
		float sum = lhs.radian() + rhs.radian();
		return Radian(sum);
	}
	// Radian angle = Radian{0.5} + Radian{0.5};	// Radian{1}

	Radian	operator-(Radian lhs, Radian rhs)
	{
		return lhs + (-rhs);
	}// Radian angle = Radian{0.5} - Radian{0.5};	// Radian{0}

	Radian	operator*(Radian lhs, float factor)
	{
		float product = lhs.radian() * factor;
		return Radian(product);
	}// Radian angle = Radian{0.5} * 3;				// Radian{1.5}

	Radian	operator/(Radian lhs, float divider)
	{
		float quotient = lhs.radian() / divider;
		return Radian(quotient);
	}// Radian angle = Radian{0.5} / 3;				// Radian{0.166...}

	inline namespace Literal
	{
		LibMath::Radian operator""_rad(long double val) { return Radian(static_cast<float>(val)); }			// Radian angle = 0.5_rad;
		LibMath::Radian operator""_rad(unsigned long long int val) { return Radian(static_cast<float>(val)); }	// Radian angle = 1_rad;
	}
}