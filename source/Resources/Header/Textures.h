#pragma once

#include <string>

#include "ResourceManager.h"

// Texture class
class Texture : public IResource
{
public:

	// Do not create empty texture
	Texture() = default;

	// Open and load image as texture
	Texture(const std::string& filename);

	// Create texture from RGB texture pointer
	Texture(std::string const& filename, GLuint* texture);

	// Set texture parameters

	void SetParameter(unsigned int param, unsigned int value) const;
	void SetParameter(unsigned int param, float value) const;


	// Bind to texture unit
	void BindToUnit(unsigned int textureUnit) const;

	// Delete texture
	~Texture();


	Texture& operator=(const Texture& rhs);

	// OpenGL texture ID
	unsigned int m_id = 0;

};


class CubeMap : public IResource
{
public:

    // Do not create empty texture
    CubeMap() = delete;

    // Open and load images from subfolder
    CubeMap(const std::string& subfolder);

    // Set texture parameters

    void SetParameter(unsigned int param, unsigned int value) const;
    void SetParameter(unsigned int param, float value) const;

	// Bind cube map
	void Bind();

	// Bind to OpenGL texture unit
	void BindToUnit(unsigned int unit);

    // Delete texture
    ~CubeMap();

    // OpenGL texture ID
    unsigned int m_id = 0;
};