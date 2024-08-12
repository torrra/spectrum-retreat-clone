#pragma once

#include "LibMath/Vector/Vector2.h"
#include "LibMath/Vector/Vector3.h"

class Vertex
{
public:
    // Create empty vertex (all values set to 0)
    Vertex() = default;

    // Copy existing vertex
    Vertex(const Vertex& other)
        : m_position(other.m_position), m_normal(other.m_normal), m_textureUV(other.m_textureUV)
    {
    }

    // Create vertex with only position as non-zero vector
    Vertex(const LibMath::Vector3& pos) : m_position(pos)
    {
    }

    // Create vertex with position and normal vectors
    Vertex(const LibMath::Vector3& pos, const LibMath::Vector3& normal)
        : m_position(pos), m_normal(normal)
    {
    }

    // Create vertex with position, normal vector and texture coordinates
    Vertex(const LibMath::Vector3& pos, const LibMath::Vector3& normal, const LibMath::Vector2& texCoords)
        : m_position(pos), m_normal(normal), m_textureUV(texCoords)
    {
    }

    // Create vertex with only position as 3 floats
    Vertex(float x, float y, float z)
    {
        m_position.m_x = x;
        m_position.m_y = y;
        m_position.m_z = z;
    }

    // Create vertex with position and texture coordinates
    Vertex(const LibMath::Vector3& pos, const LibMath::Vector2& texCoords)
        : m_position(pos), m_textureUV(texCoords)
    {
    }

    // Destroy vertex
    ~Vertex() = default;

    // Copy assignment operator
    Vertex& operator=(const Vertex& rhs)
    {
        m_position = rhs.m_position;
        m_normal = rhs.m_normal;
        m_textureUV = rhs.m_textureUV;

        return *this;
    }

	LibMath::Vector3 m_position;
	LibMath::Vector3 m_normal;
	LibMath::Vector2 m_textureUV;
};