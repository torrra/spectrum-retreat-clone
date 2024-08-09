#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "Light.h"

// Initialize the number of active lights as 0 for each type
int PointLight::m_active = 0;
int DirectionalLight::m_active = 0;
int SpotLight::m_active = 0;

void BaseLight::SetDiffuse(const LibMath::Vector4& color)
{
	// Assign the color of the diffuse lighting using a vector 4
	m_diffuseColor = color;
}

void BaseLight::SetDiffuse(float r, float g, float b, float a)
{
	// Assign the color of the diffuse lighting using the individual components
	m_diffuseColor = { r, g, b, a };
}

void BaseLight::SetAmbient(const LibMath::Vector4& color)
{
	// Assign the color of the ambient lighting using a vector 4
	m_ambientColor = color;
}

void BaseLight::SetAmbient(float r, float g, float b, float a)
{
	// Assign the ambient light color by the individual components
	m_ambientColor = { r, g, b, a };
}

void BaseLight::SetSpecular(const LibMath::Vector4& color)
{
	// Assign the color of the specular lighting using a vector 4
	m_specularColor = color;
}

void BaseLight::SetSpecular(float r, float g, float b, float a)
{
	// Assign the specular light color by the individual components
	m_specularColor = { r, g, b, a };
}

void BaseLight::SetAttenuation(const LibMath::Vector3& vec)
{
	// Assign the attenuation value using a vector 3
	m_attenuation = vec;
}

void BaseLight::SetAttenuation(float constant, float linear, float quadratic)
{
	// Assign the attenuation value via its individual components
	m_attenuation = { constant, linear, quadratic };
}

// Point light copy constructor
PointLight::PointLight(const PointLight& other)
{
	// Assign all values as a copy of the values from another point light
	m_ambientColor = other.m_ambientColor;
	m_diffuseColor = other.m_diffuseColor;
	m_specularColor = other.m_specularColor;
	m_attenuation = other.m_attenuation;
	m_position = other.m_position;

	// Increase the number of active light sources in the scene
	m_index = m_active++;
}

// Initialize point light components with a single color
void PointLight::InitPointLight(LibMath::Vector3 const& position, LibMath::Vector4 const& color)
{
	// Set the point light color for its three components (ambient, diffuse & specular)
	SetAmbient(color);
	SetDiffuse(color);
	SetSpecular(color);

	// Assign the point light's position in the scene
	SetPosition(position);
}

// Initialize point light components using different colors
void PointLight::InitPointLight(
	LibMath::Vector3 const& position,
	LibMath::Vector4 const& ambientColor,
	LibMath::Vector4 const& diffuseColor,
	LibMath::Vector4 const& SpecularColor)
{
	// Set the different point light colors for its three components (ambient, diffuse & specular)
	SetAmbient(ambientColor);
	SetDiffuse(diffuseColor);
	SetSpecular(SpecularColor);

	// Assign the point light's position in the scene
	SetPosition(position);
}

void PointLight::PointLightUI(int ID)
{
	// Allows to reuse the same ID multiple times
	ImGui::PushID(ID);

	std::string name = "Point Light ";

	// Group UI components together
	ImGui::BeginGroup();
	{
		// Title
		ImGui::SeparatorText(name.c_str());

		// Local translation
		ImGui::Text("Local Position");

		ImGui::InputFloat("x position", &m_position.m_x);
		ImGui::InputFloat("y position", &m_position.m_y);
		ImGui::InputFloat("z position", &m_position.m_z);

		ImGui::Separator();

		// Attenuation factors
		ImGui::Text("Attenuation");

		ImGui::InputFloat("Constant", &m_attenuation.m_x);
		ImGui::InputFloat("Linear", &m_attenuation.m_y);
		ImGui::InputFloat("Quadratic", &m_attenuation.m_z);

		ImGui::Separator();

		// Ambient RGB components
		ImGui::Text("Ambient");

		ImGui::InputFloat("Ambient red", &m_ambientColor.m_x);
		ImGui::InputFloat("Ambient green", &m_ambientColor.m_y);
		ImGui::InputFloat("Ambient blue", &m_ambientColor.m_z);

		ImGui::Separator();

		// Diffuse RGB components
		ImGui::Text("Diffuse");

		ImGui::InputFloat("Diffuse red", &m_diffuseColor.m_x);
		ImGui::InputFloat("Diffuse green", &m_diffuseColor.m_y);
		ImGui::InputFloat("Diffuse blue", &m_diffuseColor.m_z);

		ImGui::Separator();


		// Specular RGB components
		ImGui::Text("Specular");

		ImGui::InputFloat("Specular red", &m_specularColor.m_x);
		ImGui::InputFloat("Specular green", &m_specularColor.m_y);
		ImGui::InputFloat("Specular blue", &m_specularColor.m_z);
		ImGui::Separator();

		// Reset transformation of given mesh
		if (ImGui::Button("Reset Components"))
		{
			// Set UI components to default values
			m_position = {0.0f, 2.0f, 1.0f};
			m_attenuation = { 1.f, 0.09f, 0.032f };
			m_ambientColor = { 1.f, 1.f, 1.f, 1.f };
			m_diffuseColor = { 1.f, 1.f, 1.f, 1.f };
			m_specularColor = { 1.f, 1.f, 1.f, 1.f };
		}


		ImGui::Separator();
	}
	ImGui::EndGroup();
	// Pop IDs to allow us to re-use this function for different meshes
	ImGui::PopID();
}

void PointLight::SetValues(void)
{
}

void PointLight::SetUniforms(Shader& program)
{
	std::stringstream	stream;
	const char*			variable = "omnilights";

	/*
	*	Get the name of each uniform variable for a point light stored
	*	in our shader via the string stream & assign the variable's value
	*	with the value passed in the SetUniform function.
	*/

	m_index = m_active++;

	// Get the name for the diffuse light component as a string
	stream << variable << '[' << m_index << ']' << ".diffuse";
	// Assign the name of the diffuse light component with its value
	program.SetUniform(stream.str(), m_diffuseColor);

	// Clear the string
	stream.str(std::string());

	// Get the name for the ambient light component as a string
	stream << variable << '[' << m_index << ']' << ".ambient";
	// Assign the name of the ambient light component with its value
	program.SetUniform(stream.str(), m_ambientColor);

	// Clear the string
	stream.str(std::string());

	// Get the name for the specular light component as a string
	stream << variable << '[' << m_index << ']' << ".specular";
	// Assign the name of the specular light component with its value
	program.SetUniform(stream.str(), m_specularColor);

	// Clear the string
	stream.str(std::string());

	// Get the name for the light's attenuation
	stream << variable << '[' << m_index << ']' << ".attenuation";
	// Assign the name of the light's attenuation with its value
	program.SetUniform(stream.str(), m_attenuation);

	// Clear the string
	stream.str(std::string());

	// Get the name for the light's position
	stream << variable << '[' << m_index << ']' << ".position";
	// Set the light's position and given value
	program.SetUniform(stream.str(), m_position);

	// Clear the string
	stream.str(std::string());

	program.SetUniform("activeOmnis", m_active);

	m_enabled = false;
}

// Assign the position for a given point light via a vector 3
void PointLight::SetPosition(const LibMath::Vector3& vec)
{
	m_position = vec;
}

// Assign the position for a given point light via its x, y, z components
void PointLight::SetPosition(float x, float y, float z)
{
	m_position = { x, y, z };
}

// Directional light copy constructor
DirectionalLight::DirectionalLight(const DirectionalLight& other)
{
	// Assign copied direction light's values
	m_ambientColor = other.m_ambientColor;
	m_diffuseColor = other.m_diffuseColor;
	m_specularColor = other.m_specularColor;
	m_attenuation = other.m_attenuation;
	m_direction = other.m_direction;

	// Increase the number of lights currently active
	m_index = m_active++;
}

// Initialize a directional light using 1 color
void DirectionalLight::InitDirectionalLight(
	LibMath::Vector3 const& direction,
	LibMath::Vector4 const& color)
{
	SetAmbient(color);
	SetDiffuse(color);
	SetSpecular(color);

	SetDirection(direction);
}

// Initialize a directional light using different colors
void DirectionalLight::InitDirectionalLight(
	LibMath::Vector3 const& direction,
	LibMath::Vector4 const& ambientColor,
	LibMath::Vector4 const& diffuseColor,
	LibMath::Vector4 const& SpecularColor)
{
	SetAmbient(ambientColor);
	SetDiffuse(diffuseColor);
	SetSpecular(SpecularColor);

	SetDirection(direction);
}

void DirectionalLight::SetDirection(const LibMath::Vector3& vec)
{
	m_direction = vec;
}

void DirectionalLight::SetDirection(float x, float y, float z)
{
	m_direction = { x, y, z };
}

void DirectionalLight::DirectionalUI(int ID)
{
	// Allows to reuse the same ID multiple times
	ImGui::PushID(ID);

	std::string name = "Point Light ";

	// Group UI components together
	ImGui::BeginGroup();
	{
		// Title
		ImGui::SeparatorText(name.c_str());

		// Light direction
		ImGui::Text("Direction");

		ImGui::InputFloat("x direction", &m_direction.m_x);
		ImGui::InputFloat("y direction", &m_direction.m_y);
		ImGui::InputFloat("z direction", &m_direction.m_z);

		ImGui::Separator();

		// Attenuation factors
		ImGui::Text("Attenuation");

		ImGui::InputFloat("Constant", &m_attenuation.m_x);
		ImGui::InputFloat("Linear", &m_attenuation.m_y);
		ImGui::InputFloat("Quadratic", &m_attenuation.m_z);

		ImGui::Separator();

		// Ambient RGB components
		ImGui::Text("Ambient");

		ImGui::InputFloat("Ambient red", &m_ambientColor.m_x);
		ImGui::InputFloat("Ambient green", &m_ambientColor.m_y);
		ImGui::InputFloat("Ambient blue", &m_ambientColor.m_z);

		ImGui::Separator();

		// Diffuse RGB components
		ImGui::Text("Diffuse");

		ImGui::InputFloat("Diffuse red", &m_diffuseColor.m_x);
		ImGui::InputFloat("Diffuse green", &m_diffuseColor.m_y);
		ImGui::InputFloat("Diffuse blue", &m_diffuseColor.m_z);

		ImGui::Separator();

		// Specular RGB components
		ImGui::Text("Specular");

		ImGui::InputFloat("Specular red", &m_specularColor.m_x);
		ImGui::InputFloat("Specular green", &m_specularColor.m_y);
		ImGui::InputFloat("Specular blue", &m_specularColor.m_z);
		ImGui::Separator();

		// Reset transformation of given mesh
		if (ImGui::Button("Reset Components"))
		{
			// Set UI components to default values
			m_direction = { 0.0f, -2.0f, -1.0f };
			m_attenuation = { 1.f, 0.09f, 0.032f };
			m_ambientColor = { 1.f, 1.f, 1.f, 1.f };
			m_diffuseColor = { 1.f, 1.f, 1.f, 1.f };
			m_specularColor = { 1.f, 1.f, 1.f, 1.f };
		}


		ImGui::Separator();
	}
	ImGui::EndGroup();
	// Pop IDs to allow us to re-use this function for different meshes
	ImGui::PopID();
}

void DirectionalLight::SetUniforms(Shader& program)
{
	std::stringstream	stream;
	const char*         variable =  "dirlights";

	/*
	*	Get the name of each uniform variable for a directional light stored
	*	in our shader via the string stream & assign the variable's value
	*	with the value passed in the SetUniform function.
	*/

	m_index = m_active++;

	stream << variable << '[' << m_index << ']' << ".diffuse";
	program.SetUniform(stream.str(), m_diffuseColor);

	// Clear string
	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".ambient";
	program.SetUniform(stream.str(), m_ambientColor);

	// Clear string
	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".specular";
	program.SetUniform(stream.str(), m_specularColor);

	// Clear string
	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".attenuation";
	program.SetUniform(stream.str(), m_attenuation);

	// Clear string
	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".direction";
	program.SetUniform(stream.str(), m_direction);

	// Clear string
	stream.str(std::string());

	program.SetUniform("activeDirectionals", m_active);

	m_enabled = false;
}

// Spot light copy constructor
SpotLight::SpotLight(const SpotLight& other)
{
	// Assign each value via the other spotlight's values
	m_ambientColor = other.m_ambientColor;
	m_diffuseColor = other.m_diffuseColor;
	m_specularColor = other.m_specularColor;
	m_attenuation = other.m_attenuation;
	m_direction = other.m_direction;
	m_position = other.m_position;
	m_innerCutoff = other.m_innerCutoff;
	m_outerCutoff = other.m_outerCutoff;

	// Increase the number of active lights in the scene by 1
	m_index = m_active++;
}

// Initialize the components of a spotlight using one color
void SpotLight::InitSpotLight(
	LibMath::Vector3 const& position,
	LibMath::Vector3 const& direction,
	LibMath::Vector4 const& color)
{
	SetAmbient(color);
	SetDiffuse(color);
	SetSpecular(color);

	SetPosition(position);
	SetDirection(direction);
}

// Initialize the components of a spotlight using different colors
void SpotLight::InitSpotLight(
	LibMath::Vector3 const& position,
	LibMath::Vector3 const& direction,
	LibMath::Vector4 const& ambientColor,
	LibMath::Vector4 const& diffuseColor,
	LibMath::Vector4 const& SpecularColor)
{
	SetAmbient(ambientColor);
	SetDiffuse(diffuseColor);
	SetSpecular(SpecularColor);

	SetPosition(position);
	SetDirection(direction);
}

void SpotLight::SetDirection(const LibMath::Vector3& vec)
{
	// Assign the direction of the spotlight with a vector 3
	m_direction = vec;
}

void SpotLight::SetDirection(float x, float y, float z)
{
	// Assign the direction of a spotlight with its x, y, z components
	m_direction = { x, y, z };
}

void SpotLight::SetPosition(const LibMath::Vector3& vec)
{
	// Assign the position of a spotlight using a vector 3
	m_position = vec;
}

void SpotLight::SetPosition(float x, float y, float z)
{
	// Assign the position of a spotlight using the x, y, z components
	m_position = { x, y, z };
}

void SpotLight::SetCutoff(float inner, float outer)
{
	// Assign value for inner & outer cutoffs
	m_innerCutoff = inner;
	m_outerCutoff = outer;
}

void SpotLight::SpotlightUI(int ID)
{
	// Allows to reuse the same ID multiple times
	ImGui::PushID(ID);

	std::string name = "Point Light ";

	// Group UI components together
	ImGui::BeginGroup();
	{
		// Title
		ImGui::SeparatorText(name.c_str());

		// Local translation
		ImGui::Text("Local Position");

		ImGui::InputFloat("x position", &m_position.m_x);
		ImGui::InputFloat("y position", &m_position.m_y);
		ImGui::InputFloat("z position", &m_position.m_z);

		// Light direction
		ImGui::Text("Direction");

		ImGui::InputFloat("x direction", &m_direction.m_x);
		ImGui::InputFloat("y direction", &m_direction.m_y);
		ImGui::InputFloat("z direction", &m_direction.m_z);

		ImGui::Separator();

		// Attenuation factors
		ImGui::Text("Attenuation");

		ImGui::InputFloat("Constant", &m_attenuation.m_x);
		ImGui::InputFloat("Linear", &m_attenuation.m_y);
		ImGui::InputFloat("Quadratic", &m_attenuation.m_z);

		ImGui::Separator();

		// Ambient  RGB components
		ImGui::Text("Ambient");

		ImGui::InputFloat("Ambient red", &m_ambientColor.m_x);
		ImGui::InputFloat("Ambient green", &m_ambientColor.m_y);
		ImGui::InputFloat("Ambient blue", &m_ambientColor.m_z);

		ImGui::Separator();

		// Diffuse  RGB components
		ImGui::Text("Diffuse");

		ImGui::InputFloat("Diffuse red", &m_diffuseColor.m_x);
		ImGui::InputFloat("Diffuse green", &m_diffuseColor.m_y);
		ImGui::InputFloat("Diffuse blue", &m_diffuseColor.m_z);

		ImGui::Separator();

		// Specular RGB components
		ImGui::Text("Specular");

		ImGui::InputFloat("Specular red", &m_specularColor.m_x);
		ImGui::InputFloat("Specular green", &m_specularColor.m_y);
		ImGui::InputFloat("Specular blue", &m_specularColor.m_z);
		ImGui::Separator();

		// Reset transformation of given mesh
		if (ImGui::Button("Reset Components"))
		{
			// Set UI components to default values
			m_position = { 0.0f, 2.0f, 1.0f };
			m_direction = { 0.0f, -2.0f, -1.0f };
			m_attenuation = { 1.f, 0.09f, 0.032f };
			m_ambientColor = { 1.f, 1.f, 1.f, 1.f };
			m_diffuseColor = { 1.f, 1.f, 1.f, 1.f };
			m_specularColor = { 1.f, 1.f, 1.f, 1.f };
		}


		ImGui::Separator();
	}
	ImGui::EndGroup();
	// Pop IDs to allow us to re-use this function for different meshes
	ImGui::PopID();
}

void SpotLight::SetUniforms(Shader& program)
{
	std::stringstream	stream;
	const char*			variable = "spotlights";

	/*
	*  Get the name of each field for a spotlight via the string
	*  stream. Then assign the value in the given shader using the given
	*  value passed in the SetUniform function.
	*/

	m_index = m_active++;

	// Get name via string stream
	stream << variable << '[' << m_index << ']' << ".diffuse";
	// Assign value to the given uniform field with the given name in the shader
	program.SetUniform(stream.str(), m_diffuseColor);

	// Clear string stream
	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".ambient";
	program.SetUniform(stream.str(), m_ambientColor);

	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".specular";
	program.SetUniform(stream.str(), m_specularColor);

	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".attenuation";
	program.SetUniform(stream.str(), m_attenuation);

	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".position";
	program.SetUniform(stream.str(), m_position);

	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".direction";
	program.SetUniform(stream.str(), m_direction);

	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".innerCutoff";
	program.SetUniform(stream.str(), m_innerCutoff);

	stream.str(std::string());

	stream << variable << '[' << m_index << ']' << ".outerCutoff";
	program.SetUniform(stream.str(), m_outerCutoff);

	stream.str(std::string());

	program.SetUniform("activeSpots", m_active);

	m_enabled = false;
}

Material::Material(const std::string&, const LibMath::Vector4& color, float shininess)
	: m_ambient(color), m_diffuse(color), m_specular(color), m_shininess(shininess)
{}

Material::Material(const std::string&, const LibMath::Vector4& ambientColor, const LibMath::Vector4& diffuseColor, const LibMath::Vector4& specularColor, float shininess)
	: m_ambient(ambientColor), m_diffuse(diffuseColor), m_specular(specularColor), m_shininess(shininess)
{}

Material::Material(const std::string&, const LibMath::Vector4 & ambientColor, const LibMath::Vector4 & diffuseColor, const LibMath::Vector4 & specularColor, const LibMath::Vector4 & emissiveColor, float shininess)
	: m_ambient(ambientColor), m_diffuse(diffuseColor), m_specular(specularColor), m_shininess(shininess), m_emissive(emissiveColor)
{
}

// Initialize the material using a single color
void Material::InitMaterial(LibMath::Vector4 color, float shininess)
{
	SetAmbient(color);
	SetDiffuse(color);
	SetSpecular(color);

	SetShininess(shininess);
}

// Initialize the material using different colors
void Material::InitMaterial(
	const LibMath::Vector4& ambientColor,
	const LibMath::Vector4& diffuseColor,
	const LibMath::Vector4& specularColor,
	float			 shininess)
{
	SetAmbient(ambientColor);
	SetDiffuse(diffuseColor);
	SetSpecular(specularColor);

	SetShininess(shininess);
}

void Material::InitMaterial(const LibMath::Vector4& ambientColor, const LibMath::Vector4& diffuseColor, const LibMath::Vector4& specularColor, const LibMath::Vector4& emissiveColor, float shininess)
{
	SetAmbient(ambientColor);
	SetDiffuse(diffuseColor);
	SetSpecular(specularColor);
	SetEmissive(emissiveColor);

	SetShininess(shininess);
}

// Assign the diffuse material component via a vector 4
void Material::SetDiffuse(const LibMath::Vector4& color)
{
	m_diffuse = color;
}

// Assign the diffuse material component via its rgba components
void Material::SetDiffuse(float r, float g, float b, float a)
{
	m_diffuse = { r, g, b, a };
}

// Assign the ambient material component via a vector 4
void Material::SetAmbient(const LibMath::Vector4& color)
{
	m_ambient = color;
}

// Assign the ambient material component via its rgba components
void Material::SetAmbient(float r, float g, float b, float a)
{
	m_ambient = { r, g, b, a };
}

// Assign the specular material component via a vector 4
void Material::SetSpecular(const LibMath::Vector4& color)
{
	m_specular = color;
}

// Assign the specular material component via its rgba components
void Material::SetSpecular(float r, float g, float b, float a)
{
	m_specular = { r, g, b, a };
}

void Material::SetEmissive(const LibMath::Vector4& color)
{
	m_emissive = color;
}

void Material::SetEmissive(float r, float g, float b, float a)
{
	m_emissive = { r, g, b, a };
}

// Assign the shininess of the material
void Material::SetShininess(float shininess)
{
	m_shininess = shininess;
}

void Material::SetUniforms(Shader& program, const char* variable) const
{
	/*
	*	Store the name of each material uniform as a string before
	*	adding the suffix (.diffuse, .ambient, .specular) in order to
	*	get the complete uniform name. Then using the function
	*	SetUniform assign the values for the fields inside the shader
	*	file.
	*/

	std::string matComponent(variable);

	matComponent += ".diffuse";

	program.SetUniform(matComponent, m_diffuse);

	matComponent = variable;
	matComponent += ".ambient";

	program.SetUniform(matComponent, m_ambient);

	matComponent = variable;
	matComponent += ".specular";

	program.SetUniform(matComponent, m_specular);


	matComponent = variable;
	matComponent += ".emissive";

	program.SetUniform(matComponent, m_emissive);

	matComponent = variable;
	matComponent += ".shininess";

	program.SetUniform(matComponent, m_shininess);
}

void ResetLights()
{
	PointLight::m_active = 0;
	DirectionalLight::m_active = 0;
	SpotLight::m_active = 0;
}