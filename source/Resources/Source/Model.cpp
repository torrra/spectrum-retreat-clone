#include <iostream>
#include <filesystem>

#include "glad/glad.h"

#include "Model.h"
#include "ModelImporter.h"

Model::Model(const std::string& path)
{
    // Add subfolder path
    std::string relativePath("assets/meshes/");

    relativePath += path;

    // Open file
    std::ifstream objFile(relativePath, std::ios::in | std::ios::binary);

    // Read if successful
    if (!ImportWavefront(relativePath))
        std::cout << "Cannot open model file '" << path << "'" << std::endl;

    CreateVAO();
}



Model::~Model(void)
{
    m_ebo.Delete();
    m_vbo.Delete();
    m_vao.Delete();
}




VertexBuffer Model::CreateVBO(VAO& vao)
{
    // Return empty buffer if no vertex is stored
    if (!m_vertices.size())
        return VertexBuffer(0);

    // Bind VAO
    vao.Bind();

    // Get number of vertices to draw
    int count = static_cast<int>(m_vertices.size());

    // If no vertex is to be drawn, add them (do not update count if an EBO has already added a count)
    if (!vao.m_vertexCount)
        vao.m_vertexCount += count;

    // Create VBO and set data
    VertexBuffer vbo(count * sizeof(Vertex), &(m_vertices[0].m_position.m_x));


    // Set and enable position, normals and texture coordinates
    vao.SetAttrib(vbo, 0, 3, 8 * sizeof(float), NULL);
    vao.SetAttrib(vbo, 1, 3, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    vao.SetAttrib(vbo, 2, 2, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    vao.Unbind();

    return vbo;
}

IndexBuffer Model::CreateEBO(VAO& vao)
{
    // Return empty buffer if no index is stored
    if (!m_indices.size())
        return IndexBuffer(0);

    // Bind VAO
    vao.Bind();

    // Get number of vertices to draw
    int count = static_cast<int>(m_indices.size());

    // Assign index count to VAO
    vao.m_vertexCount = count;

    // Create VBO and set data
    vao.m_ebo = IndexBuffer(count * sizeof(unsigned int), &m_indices[0]);

    vao.Unbind();

    return vao.m_ebo;
}

bool Model::ImportWavefront(const std::filesystem::path& path)
{
    WavefrontImporter reader(this);

    return reader.LoadModel(path);
}

void Model::CreateVAO()
{
    // Create VAO
    m_vao = VertexAttributes();

    // Create buffers
    m_vbo = CreateVBO(m_vao);
    m_ebo = CreateEBO(m_vao);
}