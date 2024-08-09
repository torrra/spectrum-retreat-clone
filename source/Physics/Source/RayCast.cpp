#include "PhysicsLib/RayCast.h"

#include "LibMath/Arithmetic.h"

Ray::Ray(const LibMath::Vector3& pos, const LibMath::Vector3& dir)
	: m_origin(pos)
{
	m_direction = dir.normalizedCopy();

    // Pre compute direction divisor
	m_inverseDir.m_x = (m_direction.m_x != 0.f) ?  1.f / m_direction.m_x : 0.f;
	m_inverseDir.m_y = (m_direction.m_y != 0.f) ?  1.f / m_direction.m_y : 0.f;
	m_inverseDir.m_z = (m_direction.m_z != 0.f) ?  1.f / m_direction.m_z : 0.f;
}

bool Ray::Intersect(const BoxBV& box, float& distance) const
{
    float   maxIntersect = 1000000.f, minIntersect = -1000000.f;

    // x plane = 0, y plane = 1, z plane = 2
    for (int axis = 0; axis < 3; ++axis)
    {
        // t1 for x, t3 for y, t5 for z
        float lowIntersect = (box.m_minVertex[axis] - m_origin[axis]) * m_inverseDir[axis];

        // t2 for x, t4 for y, t6 for z
        float highIntersect = (box.m_maxVertex[axis] - m_origin[axis]) * m_inverseDir[axis];

        maxIntersect = LibMath::min(LibMath::max(lowIntersect, highIntersect), maxIntersect);
        minIntersect = LibMath::max(LibMath::min(lowIntersect, highIntersect), minIntersect);
    }

    // Check if an intersection occured
    bool intersection = maxIntersect > LibMath::max(minIntersect, 0.f);

    // Write distance into reference argument if intersection is successful, write FLT_MAX otherwise
    distance = (intersection) ? minIntersect : FLT_MAX;

    // Return intersection status
    return intersection;
}