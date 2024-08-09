#version 330

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    // Output texture coordinates
    TexCoords = aTexCoords;

    // Transformed homogenous coordinates
    gl_Position = vec4(aPos.x + 0.71f, aPos.y -0.65f, aPos.z - 0.2f, 1.0f);
}