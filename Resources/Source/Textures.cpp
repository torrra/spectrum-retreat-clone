#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <STB/stb_image.h>
#include <glad/glad.h>

#include <iostream>

#include "Textures.h"

Texture::Texture(std::string const& fileName)
{
	// Add subdirectory to file path
	std::string relativePath("assets/textures/");
	relativePath += fileName;

	// Flip texture imported upside down
	stbi_set_flip_vertically_on_load(true);
	
	// Texture info
	int				texWidth, texHeight, nbChannels;

	// Load image and get info
	unsigned char*	textureData = stbi_load(relativePath.c_str(), &texWidth, &texHeight, nullptr, 4);

	// Generate OpenGL texture if successfully loaded
	if (textureData)
	{
		// Create texture
		glGenTextures(1, &m_id);

		// Bind to set data and parameters
		glBindTexture(GL_TEXTURE_2D, m_id);

		// Set default wrap parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Set default filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Give data to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(textureData);
	}

	// Tell user loading failed
	else
	{
		std::cout << "Failed to open " << fileName << std::endl;
	}

}

Texture::Texture(std::string const& filename, GLuint* texture)
{
	// Add subdirectory to file path
	std::string relativePath("assets/textures/");
	relativePath += filename;

	// Flip texture imported upside down
	stbi_set_flip_vertically_on_load(true);

	// Texture info
	int				texWidth, texHeight;

	// Load image and get info
	unsigned char* textureData = stbi_load(relativePath.c_str(), &texWidth, &texHeight, nullptr, 4);

	// Generate OpenGL texture if successfully loaded
	if (textureData)
	{
		// Create texture
		GLuint id;
		glGenTextures(1, &id);

		// Bind to set data and parameters
		glBindTexture(GL_TEXTURE_2D, id);

		// Set default filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		// Set default wrap parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		
		// Generate mipmaps
		//glGenerateMipmap(GL_TEXTURE_2D);
		
		stbi_image_free(textureData);

		*texture = id;
	}

	// Tell user loading failed
	else
	{
		std::cout << "Failed to open " << filename << std::endl;
	}
}


void Texture::SetParameter(unsigned int param, unsigned int value) const
{
	// Bind texture and set parameter
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTextureParameteri(m_id, param, value);
}

void Texture::SetParameter(unsigned int param, float value) const
{
	// Bind texture and set parameter
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTextureParameterf(m_id, param, value);
}


void Texture::BindToUnit(unsigned int textureUnit) const
{
	// Bind texture to unit
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

Texture::~Texture()
{
	// Delete texture
	if (m_id)
		glDeleteTextures(1, &m_id);
}


Texture& Texture::operator=(const Texture& rhs)
{
	m_id = rhs.m_id;

	return *this;
}


CubeMap::CubeMap(const std::string& subfolder)
{
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

	std::string faces[6]
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};

	int width, height, nbChannels;

	for (unsigned int face = 0; face < 6; ++face)
	{
		// Add subdirectory to file path
		std::string relativePath("assets/textures/");
		relativePath += subfolder + "/" + faces[face];

		unsigned char* textureData = stbi_load(relativePath.c_str(), &width, &height, &nbChannels, 0);

		if (textureData)
		{
			if (nbChannels == 3)
			{
				glTexImage2D
				(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData
				);
			}
			else
			{
				glTexImage2D
				(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
					0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData
				);
			}
			stbi_image_free(textureData);
		}

		// Tell user loading failed
		else
		{
			std::cout << "Failed to open " << faces[face] << std::endl;
		}
	}

	// Set default filtering
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set default wrap parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMap::SetParameter(unsigned int param, unsigned int value) const
{
	glTexParameteri(GL_TEXTURE_CUBE_MAP, param, value);
}

void CubeMap::SetParameter(unsigned int param, float value) const
{
	glTexParameteri(GL_TEXTURE_CUBE_MAP, param, value);
}
void CubeMap::Bind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

void CubeMap::BindToUnit(unsigned int unit)
{
	// Bind texture to unit
	glActiveTexture(unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}


CubeMap::~CubeMap()
{
	// Delete texture
	if (m_id)
		glDeleteTextures(1, &m_id);
}