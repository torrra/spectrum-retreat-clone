#pragma once

#include "LibMath/Vector.h"

#include "ColliderHierarchy.hpp"

class Ray
{
public:
    Ray(const LibMath::Vector3& pos, const LibMath::Vector3& dir);

    bool Intersect(const BoxBV& box, float& distance) const;

    // Origin or ray
    LibMath::Vector3    m_origin;

    // Direction unit vector
    LibMath::Vector3    m_direction;

    // 1 / direction
    LibMath::Vector3    m_inverseDir;
};