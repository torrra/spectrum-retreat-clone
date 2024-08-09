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

	// Struct for easier vertex index retrieval
	class AttribIndices
	{
	public:

		// Set all indices to 0
		AttribIndices() = default;

		// Get position, texture coords and normal buffers
		AttribIndices(std::vector<LibMath::Vector3>*, std::vector<LibMath::Vector2>*, std::vector<LibMath::Vector3>*);

		// Copy another object
		AttribIndices(const AttribIndices& other);

		// Get attributes from a string within a face
		void GetVertexAttribs(const std::string& vertex);

		// Add a vertex to a VBO from retrieved indices
		void AddVertex(std::vector<Vertex>& vbo);

		// Get index
		int& operator[](int index);

		// Get index
		int operator[](int index) const;

		// Check if all indices are the same as another object
		bool operator==(const AttribIndices& rhs) const;


		// Vertex attribute vectors

		std::vector<LibMath::Vector3>* m_positionVec = nullptr;
		std::vector<LibMath::Vector3>* m_normalVec = nullptr;
		std::vector<LibMath::Vector2>* m_textureVec = nullptr;


		// Vertex attribute indices
		int	m_position = 0;
		int	m_textureUV = 0;
		int	m_normal = 0;
	};

public:

	// Empty model
	Model(void) = default;

	// Open obj file and load it into RAM
	Model(const std::string& path);

	// Delete buffers and destroy objects
	~Model(void) override
	{
		m_ebo.Delete();
		m_vbo.Delete();
		m_vao.Delete();
	}

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

	// Open and read .obj file
	void ReadWavefrontFile(std::ifstream& objFile);

};