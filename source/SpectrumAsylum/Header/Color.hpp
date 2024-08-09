#pragma once

// LibMath
#include "LibMath/Vector.h"

class Color
{
public:
	// Constructors
	Color(void) = default;

	// Copy assignment constructor
	Color(const Color& other)
	{
		m_red = other.m_red;
		m_green = other.m_green;
		m_blue = other.m_blue;
	}


	Color(float r, float g, float b, float a = 1.f)
		: m_red(r), m_green(g), m_blue(b), m_alpha(a)
	{}


	// Destructor
	~Color(void) = default;


	// Color swap
	void	Swap(Color& other)
	{
		Color temp(*this);

		*this = other;
		other = temp;
	}


	// Copy assignment operator
	Color&	operator=(const Color& rhs)
	{
		m_red = rhs.m_red;
		m_green = rhs.m_green;
		m_blue = rhs.m_blue;
		m_alpha = rhs.m_alpha;

		return *this;
	}


	// Comparator operator
	bool	operator==(const Color& rhs)
	{
		return (m_red == rhs.m_red &&
				m_green == rhs.m_green &&
				m_blue == rhs.m_blue);
	}

	// Inverse equality operator
	bool	operator!=(const Color& rhs)
	{
		return !(*this == rhs);
	}


	// Conversion
	operator	LibMath::Vector4(void) const
	{
		return LibMath::Vector4(m_red, m_green, m_blue, m_alpha);
	}


	operator	LibMath::Vector3(void) const
	{
		return LibMath::Vector3(m_red, m_green, m_blue);
	}


	// Variables
	float	m_red = 1.f;
	float	m_green = 1.f;
	float	m_blue = 1.f;
	float	m_alpha = 1.f;

}; // !Class Color