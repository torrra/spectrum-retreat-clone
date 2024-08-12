#pragma once

#include "LibMath/Vector/Vector3.h"

#include "PhysicsLib/CollisionDetection.h"

// Enum to act as an index in a Frustum object
enum PLANE
{
	NEAR,
	FAR,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

// Prefix increment plane operator (no bound checking)
PLANE& operator++(PLANE& plane);

// Postfix increment plane operator (no bound checking)
PLANE  operator++(PLANE& plane, int);

class Plane
{
public:

	// Default constructor
	Plane(void) = default;

	// Constructor to create a plane from its normal and its direction
	Plane(const LibMath::Vector3& normal, float direction);

	// Constructor to create a view frustum plane from a point and a normal
	Plane(const LibMath::Vector3& point, const LibMath::Vector3& normal);

	// Check if an AABB is intersecting or in front of a view frustum plane
	bool IntersectOrForward(const BoxBV& collider) const;

	// Find signed distance between a plane and a point
	float FindDistance(const LibMath::Vector3& point) const;

	// Default destructor
	~Plane(void) = default;

	// Copy assignment operator
	Plane& operator=(const Plane& rhs);

private:
	// Plane normal vector (normalized)
	LibMath::Vector3  m_normal;

	// Plane direction (distance from origin if part of view frustum)
	float m_direction = 0.f;
};


class Frustum
{
public:
	// Default constructor
	Frustum(void) = default;

	// Default destructor
	~Frustum(void) = default;

	// Check intersection with AABB
	bool	Intersect(const BoxBV& collider) const;

	// Array index operators
	Plane&	operator[](PLANE index);
	Plane	operator[](PLANE index) const;

	// Copy assignment operator
	Frustum& operator=(const Frustum& rhs);

private:

	// View frustum planes

	Plane	m_near;
	Plane	m_far;

	Plane	m_right;
	Plane	m_left;

	Plane	m_top;
	Plane	m_bottom;
};