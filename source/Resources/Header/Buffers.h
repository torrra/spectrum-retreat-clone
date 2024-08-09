#pragma once

#include "glad/glad.h"

#define EBO GL_ELEMENT_ARRAY_BUFFER
#define VBO GL_ARRAY_BUFFER

// Vertex or Index buffer
template <unsigned int Type = GL_ARRAY_BUFFER>
class Buffer
{
public:

	// Generate OpenGL buffer
	Buffer()
	{
		glGenBuffers(1, &m_id);
	}

	// Copy another buffer's ID
	Buffer(const Buffer& other)
		: m_id(other.m_id) {}

	// Give a placeholder ID to a buffer without generating an OpenGL buffer
	Buffer(unsigned int i)
		: m_id(i) {}


	// Generate OpenGL index buffer and feed it with data
	Buffer(size_t size, unsigned int* values, bool dynamic = false)
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(Type, m_id);

		// Init data according to usage (static/dynamic)
		if (dynamic)
			glBufferData(Type, size, values, GL_DYNAMIC_DRAW);
		else
			glBufferData(Type, size, values, GL_STATIC_DRAW);

		// Unbind buffer
		glBindBuffer(Type, 0);
	}

	// Generate OpenGL vertex buffer and feed it with data
	Buffer(size_t  size, const float* values, bool dynamic = false)
	{
		// Gen and bind before initializing data
		glGenBuffers(1, &m_id);
		glBindBuffer(Type, m_id);

		// Init data according to usage (static/dynamic)
		if (dynamic)
			glBufferData(Type, size, values, GL_DYNAMIC_DRAW);
		else
			glBufferData(Type, size, values, GL_STATIC_DRAW);

		// Unbind buffer
		glBindBuffer(Type, 0);
	}


	// Set index buffer data
	void SetData(unsigned long size, unsigned int* values, bool dynamic = false) const
	{
		// Bind buffer
		glBindBuffer(Type, m_id);


		// Init data according to usage (static/dynamic)
		if (dynamic)
			glBufferData(Type, size, values, GL_DYNAMIC_DRAW);
		else
			glBufferData(Type, size, values, GL_STATIC_DRAW);

		// Unbind buffer
		glBindBuffer(Type, 0);
	}

	// Set vertex buffer data
	void SetData(unsigned long size, const float* values, bool dynamic = false) const
	{
		// Bind buffer
		glBindBuffer(Type, m_id);


		// Init data according to usage (static/dynamic)
		if (dynamic)
			glBufferData(Type, size, values, GL_DYNAMIC_DRAW);
		else
			glBufferData(Type, size, values, GL_STATIC_DRAW);

		// Unbind buffer
		glBindBuffer(Type, 0);
	}

	// Bind buffer
	void Bind() const
	{
		glBindBuffer(Type, m_id);
	}

	// Unbind buffer
	void Unbind() const
	{
		glBindBuffer(Type, 0);
	}

	// Delete OpenGL buffer
	void Delete()
	{
		//
		if (m_id)
			glDeleteBuffers(1, &m_id);
	}


private:

	// OpenGL buffer ID
	unsigned int m_id = 0;
};


// Vertex array object (VAO). Stores and organized vertex attributes
class VertexAttributes
{
public:

	// Generate empty VAO
	VertexAttributes();

	~VertexAttributes()
	{

	}

	// Give placeholder ID
	VertexAttributes(unsigned int id);

	// Increate vertex count for drawing
	void AddVertices(int count);

	// Increate vertex count for drawing
	void AddVertices(size_t count);

	// Draw vertices as-is without an index buffer
	void DrawUnindexed();

	// Draw vertices with an index buffer
	void DrawIndexed();

	// Bind VAO
	void Bind();

	// Undind VAO
	void Unbind();

	// Delete VAO and associated buffers
	void Delete();

	// Set and enable vertex attribute from a VBO
	void SetAttrib(const Buffer<VBO>& vbo, unsigned int pos, int size, int stride, void* offset);

	// Number of vertices
	int				m_vertexCount = 0;

	// EBO for indexed drawing
	Buffer<EBO>     m_ebo = 0;

private:

	// VAO ID
	unsigned int	m_id = 0;
};

// Aliases for readability/comfort
using VertexBuffer = Buffer<VBO>;
using IndexBuffer = Buffer<EBO>;
using VAO = VertexAttributes;