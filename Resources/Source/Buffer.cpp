#include "Buffers.h"

VertexAttributes::VertexAttributes()
{
	// Generate and bind VAO
	glGenVertexArrays(1, &m_id);
	glBindVertexArray(m_id);
}


VertexAttributes::VertexAttributes(unsigned int id)
	: m_id(id)
{}


void VertexAttributes::SetAttrib(const Buffer<VBO>& vbo, unsigned int pos, int size, int stride, void* offset)
{
	// Bind VAO
	vbo.Bind();

	// Set attrib pointer
	glVertexAttribPointer(pos, size, GL_FLOAT, GL_FALSE, stride, offset);

	// Enable attrib
	glEnableVertexAttribArray(pos);

	vbo.Unbind();

}

void VertexAttributes::AddVertices(int count)
{
	// Add vertices to draw
	m_vertexCount += count;
}

void VertexAttributes::AddVertices(size_t count)
{
	// Add vertices to draw
	m_vertexCount += static_cast<int>(count);
}

void VertexAttributes::DrawUnindexed()
{
	// Bind VAO and draw without EBO
	glBindVertexArray(m_id);
	glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
}

void VertexAttributes::DrawIndexed()
{
	// Bind VAO and EBO
	glBindVertexArray(m_id);

	m_ebo.Bind();

	// Draw with indices
	glDrawElements(GL_TRIANGLES, m_vertexCount, GL_UNSIGNED_INT, 0);
}

void VertexAttributes::Bind()
{
	// Bind VAO
	glBindVertexArray(m_id);
}

void VertexAttributes::Unbind()
{
	// Unbind VAO
	glBindVertexArray(0);
}

void VertexAttributes::Delete()
{
	// Delete VAO
	if (m_id)
		glDeleteVertexArrays(1, &m_id);
}