#version 330

in vec2 TexCoords;

out vec4 FragColor;

// One texture per mesh
uniform sampler2D               meshTexture;

// Adapt base color if mesh is not textured
uniform int isTextured;

uniform vec3 Color;


void main()
{
	// Fragment color
	vec4 finalColor = vec4(Color, 1.0f);
//	vec4 finalColor;

	if (isTextured != 0.f)
			finalColor *= texture(meshTexture, TexCoords);

	FragColor = finalColor;
}