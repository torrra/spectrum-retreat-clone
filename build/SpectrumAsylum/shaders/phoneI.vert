#version 330

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    // Output texture coordinates
    TexCoords = aTexCoords;

    // Transformed homogenous coordinates
    gl_Position = vec4(aPos.x + 0.7f, aPos.y -0.7f, aPos.z + 0.f, 1.0f);
}