#include "PhysicsLib/Frustum.h"
#include "LibMath/Arithmetic.h"

Plane::Plane(const LibMath::Vector3& normal, float direction)
	: m_normal(normal), m_direction(direction)
{}

Plane::Plane(const LibMath::Vector3& point, const LibMath::Vector3& normal)
	: m_normal(normal)
{
	// Init members

	m_normal.normalize();

	m_direction = -point.magnitude();
}

Plane&	Frustum::operator[](PLANE index)
{
	// Access plane with PLANE enum as index
	switch (index)
	{
	case NEAR:
		return m_near;
	case FAR:
		return m_far;
	case LEFT:
		return m_left;
	case RIGHT:
		return m_right;
	case TOP:
		return m_top;

	case BOTTOM:
	default:
		return m_bottom;
	}
}


Plane Frustum::operator[](PLANE index) const
{
	// Access plane with PLANE enum as index
	switch (index)
	{
	case NEAR:
		return m_near;
	case FAR:
		return m_far;
	case LEFT:
		return m_left;
	case RIGHT:
		return m_right;
	case TOP:
		return m_top;

	case BOTTOM:
	default:
		return m_bottom;
	}
}



Plane& Plane::operator=(const Plane& rhs)
{
	// Copy rhs plane members and return current object
	m_normal = rhs.m_normal;
	m_direction = rhs.m_direction;

	return *this;
}

Frustum& Frustum::operator=(const Frustum& rhs)
{
	// Copy all planes in rhs frustum and return current object

	for (PLANE plane = NEAR; plane <= BOTTOM; ++plane)
	{
		(*this)[plane] = rhs[plane];
	}

	return *this;
}

PLANE& operator++(PLANE& plane)
{
	// Add 1 (FAR) to current plane
	return plane = static_cast<PLANE>(plane + FAR);
}

PLANE  operator++(PLANE& plane, int)
{
	// Store plane pre increment
	PLANE preIncrement = plane;

	// Increment
	++plane;

	// Return old value
	return preIncrement;
}

bool Frustum::Intersect(const BoxBV& collider) const
{
	// Check intersection on all 6 planes
	for (PLANE plane = NEAR; plane <= BOTTOM; ++plane)
	{
		if (!(*this)[plane].IntersectOrForward(collider))
			return false;
	}

	// Return true if all tests passed
	return true;
}


float Plane::FindDistance(const LibMath::Vector3& point) const
{
	// Subtract direction from normal * point dot product
	return m_normal.dot(point) - m_direction;
}

bool Plane::IntersectOrForward(const BoxBV& collider) const
{
	// Project box onto plane and find interval
	float interval = collider.m_boxScale.m_x * LibMath::absolute(m_normal.m_x) +
				     collider.m_boxScale.m_y * LibMath::absolute(m_normal.m_y) +
				     collider.m_boxScale.m_z * LibMath::absolute(m_normal.m_z);

	/*
		Box is inside of in front of the plane if the interval radius(minus box scale for some extra leeway)
		is inferior to the signed distance between the AABB and the plane
	*/
	return -(interval + collider.m_boxScale.magnitude()) <= FindDistance(collider.m_position);
}
