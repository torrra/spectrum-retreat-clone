#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Mesh transform matrix for fragment position interpolation
uniform mat4 model;

// Normal matrix for normal vector transformation
uniform mat3 normalMat;

// Model-view-projection matrix
uniform mat4 mvp;

// Fragment position output
out vec3 FragPos;

// Normal vector output
out vec3 Normal;

// Texture coordinate output
out vec2 TexCoords;

void main()
{
    // Output fragment position
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Output normal vector
    Normal = normalMat * aNormal;

    // Output texture coordinates
    TexCoords = aTexCoords;

    // Transformed homogenous coordinates
    gl_Position = mvp * vec4(aPos, 1.0f);
}