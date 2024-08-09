#include <iostream>
#include <limits>

#include "PhysicsLib/CollisionDetection.h"

#include "LibMath/Arithmetic.h"

#define ENABLE_VERTEX_DEBUG 0
#define ENABLE_DEBUG 0

PhysicsLib::BoxCollider::BoxCollider(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, COLLIDER_TYPE type)
{
	// Set type & transform for AABB collider
	m_type = type;
	m_position = position;
	m_boxScale = boxScale;

	// Calculate box collider vertex min & max positions
	m_minVertex = m_position - m_boxScale;
	m_maxVertex = m_position + m_boxScale;
}

void PhysicsLib::BoxCollider::SetColliderTransform(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale)
{
	// Set transform for collider
	m_position = position;
	m_boxScale = boxScale;

	// Calculate box collider vertex min & max positions
	m_minVertex = m_position - m_boxScale;
	m_maxVertex = m_position + m_boxScale;

#if ENABLE_VERTEX_DEBUG == 1
	std::printf("position: {%f, %f, %f}\n", m_position.m_x, m_position.m_y, m_position.m_z);
	std::printf("scale: {%f, %f, %f}\n\n", boxScale.m_x, boxScale.m_y, boxScale.m_z);
	std::printf("Min: {%f, %f, %f}\n", m_minVertex.m_x, m_minVertex.m_y, m_minVertex.m_z);
	std::printf("Max: {%f, %f, %f}\n\n", m_maxVertex.m_x, m_maxVertex.m_y, m_maxVertex.m_z);
#endif
}

bool PhysicsLib::BoxCollider::CheckCollision(BoxCollider const& box1, BoxCollider const& box2)
{
	// Collision detection between two boxes

	// Check if either colliders are disabled
	if (!box1.m_enabled || !box2.m_enabled)
		return false;

	// Check for collision (checking for intersection between the two colliders)
	else if (box1.m_minVertex.m_x <= box2.m_maxVertex.m_x &&
		box1.m_maxVertex.m_x >= box2.m_minVertex.m_x &&
		box1.m_minVertex.m_y <= box2.m_maxVertex.m_y &&
		box1.m_maxVertex.m_y >= box2.m_minVertex.m_y &&
		box1.m_minVertex.m_z <= box2.m_maxVertex.m_z &&
		box1.m_maxVertex.m_z >= box2.m_minVertex.m_z)
	{
#if ENABLE_DEBUG == 1
		std::printf("COLLISION\n");
#endif
		// Collision occurred
		return true;
	}

#if ENABLE_DEBUG == 1
	std::printf("NO COLLISION\n");
#endif

	// Condition not met therefore, no collision
	return false;
}

bool PhysicsLib::BoxCollider::CheckCollision(BoxCollider const& box2)
{
	// Collision detection between two boxes

	// Check for collision (checking for intersection between the two colliders)
	if (m_minVertex.m_x <= box2.m_maxVertex.m_x &&
		m_maxVertex.m_x >= box2.m_minVertex.m_x &&
		m_minVertex.m_y <= box2.m_maxVertex.m_y &&
		m_maxVertex.m_y >= box2.m_minVertex.m_y &&
		m_minVertex.m_z <= box2.m_maxVertex.m_z &&
		m_maxVertex.m_z >= box2.m_minVertex.m_z)
	{
#if ENABLE_DEBUG == 1
		std::printf("COLLISION\n");
#endif
		// Collision occurred
		return true;
	}

#if ENABLE_DEBUG == 1
	std::printf("NO COLLISION\n");
#endif

	// Condition not met therefore, no collision
	return false;
}

bool PhysicsLib::BoxCollider::CheckCollision(SphereCollider const& sphereCollider)
{
	// Collision detection between box & sphere

	LibMath::Vector3	axis = {0.0f, 0.0f, 0.0f};
	float	distance = 0.0f;

	// Get box vertex closest to sphere center
	axis.m_x = LibMath::max(m_minVertex.m_x, LibMath::min(sphereCollider.m_position.m_x, m_maxVertex.m_x));
	axis.m_y = LibMath::max(m_minVertex.m_y, LibMath::min(sphereCollider.m_position.m_y, m_maxVertex.m_y));
	axis.m_z = LibMath::max(m_minVertex.m_z, LibMath::min(sphereCollider.m_position.m_z, m_maxVertex.m_z));

	// Get distance
	distance = LibMath::squareRoot(
		(axis.m_x - sphereCollider.m_position.m_x) * (axis.m_x - sphereCollider.m_position.m_x) +
		(axis.m_y - sphereCollider.m_position.m_y) * (axis.m_y - sphereCollider.m_position.m_y) +
		(axis.m_z - sphereCollider.m_position.m_z) * (axis.m_z - sphereCollider.m_position.m_z));

	// Check if distance if further than the radius which indicates if the box is inside the sphere
	return distance < sphereCollider.m_radius;
}

PhysicsLib::SphereCollider::SphereCollider(float const& radius, LibMath::Vector3 const& position)
{
	// Set collider type
	m_type = SPHERE;

	// Set collider position
	m_position = position;

	// Set collider size
	m_radius = radius;
}

void PhysicsLib::SphereCollider::SetColliderPosition(LibMath::Vector3 const& position)
{
	// Set sphere collider position
	m_position = position;
}

bool PhysicsLib::SphereCollider::CheckCollision(SphereCollider const& sphere1, SphereCollider const& sphere2)
{
	// Collision detection between two spheres

	// Calculate distance between the two spheres
	float	distance = LibMath::squareRoot(
		(sphere1.m_position.m_x - sphere2.m_position.m_x) * (sphere1.m_position.m_x - sphere2.m_position.m_x) +
		(sphere1.m_position.m_y - sphere2.m_position.m_y) * (sphere1.m_position.m_y - sphere2.m_position.m_y) +
		(sphere1.m_position.m_z - sphere2.m_position.m_z) * (sphere1.m_position.m_z - sphere2.m_position.m_z)
	);

	// Check if the distance is smaller than both radiuses & return result
	return distance < sphere1.m_radius + sphere2.m_radius;
}

bool PhysicsLib::SphereCollider::CheckCollision(SphereCollider const& sphere2)
{
	// Collision detection between two spheres

	// Calculate distance between the two spheres
	float	distance = LibMath::squareRoot(
		(m_position.m_x - sphere2.m_position.m_x) * (m_position.m_x - sphere2.m_position.m_x) +
		(m_position.m_y - sphere2.m_position.m_y) * (m_position.m_y - sphere2.m_position.m_y) +
		(m_position.m_z - sphere2.m_position.m_z) * (m_position.m_z - sphere2.m_position.m_z)
	);

	// Check if the distance is smaller than both radiuses & return result
	return distance < m_radius + sphere2.m_radius;
}

bool PhysicsLib::SphereCollider::CheckCollision(BoxCollider const& boxCollider)
{
	// Check collision between box & sphere

	LibMath::Vector3	axis = {0.0f, 0.0f, 0.0f};
	float	distance = 0.0f;

	// Get box vertex closest to sphere center
	axis.m_x = LibMath::max(boxCollider.m_minVertex.m_x, LibMath::min(m_position.m_x, boxCollider.m_maxVertex.m_x));
	axis.m_y = LibMath::max(boxCollider.m_minVertex.m_y, LibMath::min(m_position.m_y, boxCollider.m_maxVertex.m_y));
	axis.m_z = LibMath::max(boxCollider.m_minVertex.m_z, LibMath::min(m_position.m_z, boxCollider.m_maxVertex.m_z));

	// Get distance
	distance = LibMath::squareRoot(
		(axis.m_x - m_position.m_x) * (axis.m_x - m_position.m_x) +
		(axis.m_y - m_position.m_y) * (axis.m_y - m_position.m_y) +
		(axis.m_z - m_position.m_z) * (axis.m_z - m_position.m_z));

	// Check if distance if further than the radius which indicates if the box is inside the sphere
	return distance <= m_radius;
}

bool PhysicsLib::SphereCollider::CheckCollision(SphereCollider const& sphereCollider, BoxCollider const& boxCollider)
{
	// Check collision between box & sphere
	
	// Check collider is enabled
	if (!boxCollider.m_enabled)
		return false;

	LibMath::Vector3	axis = {0.0f, 0.0f, 0.0f};
	float	distance = 0.0f;

	// Get box vertex closest to sphere center
	axis.m_x = LibMath::max(boxCollider.m_minVertex.m_x, LibMath::min(sphereCollider.m_position.m_x, boxCollider.m_maxVertex.m_x));
	axis.m_y = LibMath::max(boxCollider.m_minVertex.m_y, LibMath::min(sphereCollider.m_position.m_y, boxCollider.m_maxVertex.m_y));
	axis.m_z = LibMath::max(boxCollider.m_minVertex.m_z, LibMath::min(sphereCollider.m_position.m_z, boxCollider.m_maxVertex.m_z));

	// Get distance
	distance = LibMath::squareRoot(
		(axis.m_x - sphereCollider.m_position.m_x) * (axis.m_x - sphereCollider.m_position.m_x) +
		(axis.m_y - sphereCollider.m_position.m_y) * (axis.m_y - sphereCollider.m_position.m_y) +
		(axis.m_z - sphereCollider.m_position.m_z) * (axis.m_z - sphereCollider.m_position.m_z));

	// Check if distance if further than the radius which indicates if the box is inside the sphere
	return distance <= sphereCollider.m_radius;
}

PhysicsLib::ColoredBoxCollider::ColoredBoxCollider(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, COLLIDER_TYPE type)
{
	// Set collider type
	m_type = type;

	// Set collider transform
	m_position = position;
	m_boxScale = boxScale;

	// Calculate box collider vertex min & max positions
	m_minVertex =
	{
		(m_position.m_x - (m_boxScale.m_x / 2.0f)),
		(m_position.m_y - (m_boxScale.m_y / 2.0f)),
		(m_position.m_z - (m_boxScale.m_z / 2.0f))
	};

	m_maxVertex =
	{
		(m_position.m_x + (m_boxScale.m_x / 2.0f)),
		(m_position.m_y + (m_boxScale.m_y / 2.0f)),
		(m_position.m_z + (m_boxScale.m_z / 2.0f))
	};
}

PhysicsLib::HoledCollider::HoledCollider(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, COLLIDER_TYPE type)
{
	// Set collider type
	m_type = type;

	// Set collider transform
	m_position = position;
	m_boxScale = boxScale;

	// Calculate box collider vertex min & max positions
	m_minVertex = m_position - m_boxScale;
	m_maxVertex = m_position + m_boxScale;
}

PhysicsLib::Teleporter::Teleporter(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, COLLIDER_TYPE type)
{
	// Set collider type
	m_type = type;

	// Set collider transform
	m_position = position;
	m_boxScale = boxScale;

	// Calculate box collider vertex min & max positions
	m_minVertex = m_position - m_boxScale;
	m_maxVertex = m_position + m_boxScale;
}
