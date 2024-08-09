#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "ResourceManager.h"
#include "Vertex.h"
#include "Buffers.h"

#include "Textures.h"


class Model : public IResource
{
private:

	class MetaData;
	class WavefrontImporter;

public:

	// Empty model
	Model(void) = default;

	// Open obj file and load it into RAM
	Model(const std::string& path);

	// Delete buffers and destroy objects
	~Model(void) override;

	// Create VBO, EBO and VAO from stored data
	void			CreateVAO();

	// Create only VBO from stored data
	VertexBuffer	CreateVBO(VAO& vao);

	// Create only index buffer from stored data
	IndexBuffer		CreateEBO(VAO& vao);

	// Vertex data and indices
	std::vector<Vertex>		m_vertices;
	std::vector<uint32_t>	m_indices;

	Texture*				m_texture = nullptr;

	// Store buffer objects for easier drawings

	VertexAttributes		m_vao = 0;
	VertexBuffer			m_vbo = 0;
	IndexBuffer				m_ebo = 0;

private:

	// Read from a wavefront OBJ file
	bool			ImportWavefront(const std::filesystem::path& path);

};