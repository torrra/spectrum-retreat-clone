#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "glad/glad.h"

#include "Model.h"

#define FOURTH_VERTEX 3

Model::Model(const std::string& path)
{
    // Add subfolder path
    std::string relativePath("assets/meshes/");

    relativePath += path;

    // Open file
    std::ifstream objFile(relativePath, std::ios::in | std::ios::binary);

    // Read if successful
    if (objFile)
        ReadWavefrontFile(objFile);
    else
        std::cout << "Cannot open model file '" << path << "'" << std::endl;

    CreateVAO();
}

void Model::ReadWavefrontFile(std::ifstream& objFile)
{
    // Unordered map to store unique position/UV/normal combinations
    std::unordered_map<std::string, int> uniqueVertices;

    // Line to read and line prefix (v, vt, vn, f)
    std::string line, lineType;

    // Vectors to store vertex attributes
    std::vector<LibMath::Vector3>   tmpPos;
    std::vector<LibMath::Vector3>   tmpNormal;
    std::vector<LibMath::Vector2>   tmpTextureUV;

    // Parse entire file
    while (std::getline(objFile, line))
    {
        // v = position
        if (line.find("v ") != std::string::npos)
        {
            // Store line into stream for easier processing
            std::istringstream lineStream(line);

            // Vector components
            float x, y, z;

            // Divide line into prefix + x + y + z
            lineStream >> lineType >> x >> y >> z;

            // Add new position vector to our buffer
            tmpPos.push_back({ x, y, z });
        }

        // vn = vertex normal
        else if (line.find("vn") != std::string::npos)
        {
            // Store line into stream for easier processing
            std::istringstream lineStream(line);

            // Vector components
            float x, y, z;

            // Divide line into prefix + x + y + z
            lineStream >> lineType >> x >> y >> z;

            // Add new normal vector to our buffer
            tmpNormal.push_back({ x, y, z });
        }

        // vt = texture coordinates
        else if (line.find("vt") != std::string::npos)
        {
            // Store line into stream for easier processing
            std::istringstream lineStream(line);

            // Vector components
            float u, v;

            // Divide line into prefix + u + v
            lineStream >> lineType >> u >> v;

            // Add new coordinates vector to our buffer
            tmpTextureUV.push_back({ u, v });
        }

        // f = face
        else if (line.find("f ") != std::string::npos)
        {
            // Store line into stream for easier processing
            std::istringstream lineStream(line);

            // Vertices in face
            std::string vertices[4];

            // Get rid of line prefix
            lineStream >> lineType;

            // Store pointer to vectos for easier access
            AttribIndices attribs(&tmpPos, &tmpTextureUV, &tmpNormal);

            for (int vertID = 0; vertID < 4; ++vertID)
            {
                lineStream >> vertices[vertID];

                if (vertices[vertID].size())
                {
                    // Get vertex attributes
                    attribs.GetVertexAttribs(vertices[vertID]);

                    // Check if vertex is unique
                    auto foundVertex = uniqueVertices.find(vertices[vertID]);

                    // Push back index if it is not unique
                    if (foundVertex != uniqueVertices.end())
                        m_indices.push_back(foundVertex->second);

                    // Create new vertex if it is unique
                    else
                    {
                        // Get new vertex index
                        int newIndex = uniqueVertices.size();

                        // Store its index
                        m_indices.push_back(newIndex);

                        // Add new vertex to VBO
                        attribs.AddVertex(m_vertices);

                        // Add new vertex to unique vertex umap
                        uniqueVertices[vertices[vertID]] = newIndex;
                    }

                    if (vertID == FOURTH_VERTEX)
                    {
                        m_indices.push_back(uniqueVertices[vertices[2]]);
                        m_indices.push_back(uniqueVertices[vertices[0]]);
                    }
                }
            }

        }

    }
}

void Model::AttribIndices::GetVertexAttribs(const std::string& vertex)
{
    // Index of each attribute
    std::string index;

    // 0 = position, 1 = texture UV, 2 = vertex normal
    int indexCount = 0;

    // String length
    size_t len = vertex.size();

    // Parse entire string
    for (size_t i = 0; i < len; ++i)
    {
        // Read character
        char c = vertex[i];

        // Update index if it is not a digit
        if (c > '9' || c < '0' )
        {
            // Keep reading if it is a sign
            if (c == '+' || c == '-')
            {
                index += c;
                continue;
            }

            // Convert index string into integral index
            int stringToInt = std::atoi(index.c_str());

            // Get actual index if read index is negative
            if (stringToInt < 0)
            {
                // Add size depending on what attribute is being read
                switch (indexCount)
                {
                case 0:
                    stringToInt += static_cast<int>(m_positionVec->size());
                    break;
                case 1:
                    stringToInt += static_cast<int>(m_textureVec->size());
                    break;
                default:
                    stringToInt += static_cast<int>(m_normalVec->size());
                    break;
                }

                // Update attribute value
                (*this)[indexCount] = stringToInt;
            }

            // Do not perform any operation before updating if index is equal to 0
            else if (stringToInt == 0)
                (*this)[indexCount] = stringToInt;

            // Subtract one if index is positive and greater than 0
            else
                (*this)[indexCount] = stringToInt - 1;

            // Clear index to read next attribute index
            index.clear();

            // Read next index
            ++indexCount;
        }
        else if (i == len - 1ull || vertex[i] == '/')
        {
            // Add last character to index string
            index += c;

            // Keep reading if it is a sign
            if (c == '+' || c == '-')
                continue;

            // Convert index string into integral index
            int stringToInt = std::atoi(index.c_str());

            // Get actual index if read index is negative
            if (stringToInt < 0)
            {
                // Add size depending on what attribute is being read
                switch (indexCount)
                {
                case 0:
                    stringToInt += static_cast<int>(m_positionVec->size());
                    break;
                case 1:
                    stringToInt += static_cast<int>(m_textureVec->size());
                    break;
                default:
                    stringToInt += static_cast<int>(m_normalVec->size());
                    break;
                }

                // Update attribute value
                (*this)[indexCount] = stringToInt;
            }
            // Do not perform any operation before updating if index is equal to 0
            else if (stringToInt == 0)
                (*this)[indexCount] = stringToInt;

            // Subtract one if index is positive and greater than 0
            else
                (*this)[indexCount] = stringToInt - 1;

        }

        // Add character to index string and keep reading
        else
            index += c;
    }
}

void Model::AttribIndices::AddVertex(std::vector<Vertex>& vbo)
{
    // Check if this model has vertex normals and texture coordinates
    size_t nsize = m_normalVec->size();
    size_t tsize = m_textureVec->size();

    // If model has all 3 attributes, add all three
    if (nsize && tsize)
        vbo.emplace_back((*m_positionVec)[m_position], (*m_normalVec)[m_normal], (*m_textureVec)[m_textureUV]);

    // Add only position and normal vector if no texture coordinates was found
    else if (nsize)
        vbo.emplace_back((*m_positionVec)[m_position], (*m_normalVec)[m_normal]);

    // Add only position and texture coordinates if no normal vector was found
    else if (tsize)
        vbo.emplace_back((*m_positionVec)[m_position], (*m_textureVec)[m_textureUV]);

    // Only add position if it is the only attribute
    else
        vbo.push_back((*m_positionVec)[m_position]);

}

int& Model::AttribIndices::operator[](int index)
{
    // Get class member depending on index (used in GetVertexAttribs)
    switch (index)
    {
    case 0:
        return m_position;
    case 1:
        return m_textureUV;
    default:
        return m_normal;
    }
}

int Model::AttribIndices::operator[](int index) const
{
    // Get class member depending on index
    switch (index)
    {
    case 0:
        return m_position;
    case 1:
        return m_textureUV;
    default:
        return m_normal;
    }
}

bool Model::AttribIndices::operator==(const AttribIndices& rhs) const
{
    // Check if all members are the same between the two objects
    return (m_position == rhs.m_position) && (m_textureUV == rhs.m_textureUV) && (m_normal == rhs.m_normal);
}

Model::AttribIndices::AttribIndices(std::vector<LibMath::Vector3>* pos, std::vector<LibMath::Vector2>* tex, std::vector<LibMath::Vector3>* norm)
    : m_positionVec(pos), m_textureVec(tex), m_normalVec(norm)
{}

Model::AttribIndices::AttribIndices(const AttribIndices& other)
    : m_positionVec(other.m_positionVec), m_textureVec(other.m_textureVec), m_normalVec(other.m_normalVec),
    m_textureUV(other.m_textureUV), m_position(other.m_position), m_normal(other.m_normal)
{}

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

void Model::CreateVAO()
{
    // Create VAO
    m_vao = VertexAttributes();

    // Create buffers
    m_vbo = CreateVBO(m_vao);
    m_ebo = CreateEBO(m_vao);
}