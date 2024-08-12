#pragma once

#include "LibMath/Vector.h"

#include "SceneNode.hpp"
#include "Shader.h"

class BaseLight : public ISceneObject
{
public:
	// Constructors
			BaseLight() { m_type = LIGHT; };
			BaseLight(const std::string&) { m_type = LIGHT; }
			BaseLight(const BaseLight&) { m_type = LIGHT; }

	// Setter functions
	void	SetDiffuse(const LibMath::Vector4& color);
	void	SetDiffuse(float r, float g, float b, float a);

	void	SetAmbient(const LibMath::Vector4& color);
	void	SetAmbient(float r, float g, float b, float a);

	void	SetSpecular(const LibMath::Vector4& color);
	void	SetSpecular(float r, float g, float b, float a);

	void	SetAttenuation(const LibMath::Vector3& vec);
	void	SetAttenuation(float constant, float linear, float quadratic);

	// Update shader function
	virtual void	SetUniforms(Shader& program) = 0;

	LibMath::Vector4	m_diffuseColor;
	LibMath::Vector4	m_ambientColor;
	LibMath::Vector4	m_specularColor;

	LibMath::Vector3	m_attenuation;

	int					m_index = 0;
	bool				m_enabled = true;
};

class PointLight : public BaseLight
{
public:
	// Constructors
			PointLight() { /*m_index = m_active++;*/ }
			PointLight(const std::string&) { /*m_index = m_active++;*/ }
			PointLight(const PointLight& other);

	// Initializer functions
	void	InitPointLight(LibMath::Vector3 const& position, LibMath::Vector4 const& color);
	void	InitPointLight(
				LibMath::Vector3 const& position,
				LibMath::Vector4 const& ambientColor,
				LibMath::Vector4 const& diffuseColor,
				LibMath::Vector4 const& SpecularColor
	);

	// UI functions
	void	PointLightUI(int ID);
	void	SetValues(void);

	// Update shader function
	void	SetUniforms(Shader& program) override;

	// Setter functions
	void	SetPosition(const LibMath::Vector3& vec);
	void	SetPosition(float x, float y, float z);


	LibMath::Vector3	m_position;

	// Store the number of active point lights
	static int			m_active;
};

class DirectionalLight final : public BaseLight
{
public:
	// Constructors
	DirectionalLight() = default;
			DirectionalLight(const std::string&) {}
			DirectionalLight(const DirectionalLight& other);

	// Initializer functions
	void	InitDirectionalLight(
		LibMath::Vector3 const& direction,
		LibMath::Vector4 const& color
	);
	void	InitDirectionalLight(
				LibMath::Vector3 const& direction,
				LibMath::Vector4 const& ambientColor,
				LibMath::Vector4 const& diffuseColor,
				LibMath::Vector4 const& SpecularColor
	);

	// Setter functions
	void	SetDirection(const LibMath::Vector3& vec);
	void	SetDirection(float x, float y, float z);

	void DirectionalUI(int ID);

	// Update shader function
	void	SetUniforms(Shader& program) override;

	LibMath::Vector3	m_direction;

	// Store the number of active directional lights
	static int			m_active;

};

class SpotLight final : public BaseLight
{
public:
	// Constructors
			SpotLight() = default;
			SpotLight(const std::string&) {}
			SpotLight(const SpotLight& other);

	// Initializer functions
	void	InitSpotLight(
		LibMath::Vector3 const& position,
		LibMath::Vector3 const& direction,
		LibMath::Vector4 const& color
	);
	void	InitSpotLight(
		LibMath::Vector3 const& position,
		LibMath::Vector3 const& direction,
		LibMath::Vector4 const& ambientColor,
		LibMath::Vector4 const& diffuseColor,
		LibMath::Vector4 const& SpecularColor
	);

	// Setter functions
	void	SetDirection(const LibMath::Vector3& vec);
	void	SetDirection(float x, float y, float z);

	void	SetPosition(const LibMath::Vector3& vec);
	void	SetPosition(float x, float y, float z);

	void	SetCutoff(float inner, float outer);

	void	SpotlightUI(int ID);

	// Update shader function
	void	SetUniforms(Shader& program) override;

	LibMath::Vector3	m_position;
	LibMath::Vector3	m_direction;

	float				m_innerCutoff = 0.0f;
	float				m_outerCutoff = 0.0f;

	// Store the number of active spot lights
	static int			m_active;
};

class Material : public IResource
{
public:
	// Constructor
			Material() = default;
			Material(const std::string&) {}
			Material(const std::string&, const LibMath::Vector4& color, float shininess);

			Material(const std::string&,
				const LibMath::Vector4& ambientColor,
				const LibMath::Vector4& diffuseColor,
				const LibMath::Vector4& specularColor,
				float shininess
			);


			Material(const std::string&,
				const LibMath::Vector4& ambientColor,
				const LibMath::Vector4& diffuseColor,
				const LibMath::Vector4& specularColor,
				const LibMath::Vector4& emissiveColor,
				float shininess
			);

	// Destructor
			~Material() = default;

	// Initializer functions
	void	InitMaterial(LibMath::Vector4 color, float shininess);
	void	InitMaterial(
				const LibMath::Vector4& ambientColor,
				const LibMath::Vector4& diffuseColor,
				const LibMath::Vector4& specularColor,
				float shininess
	);


	void	InitMaterial(
		const LibMath::Vector4& ambientColor,
		const LibMath::Vector4& diffuseColor,
		const LibMath::Vector4& specularColor,
		const LibMath::Vector4& emissiveColor,
		float shininess
	);

	// Setter function
	void	SetDiffuse(const LibMath::Vector4& color);
	void	SetDiffuse(float r, float g, float b, float a);

	void	SetAmbient(const LibMath::Vector4& color);
	void	SetAmbient(float r, float g, float b, float a);

	void	SetSpecular(const LibMath::Vector4& color);
	void	SetSpecular(float r, float g, float b, float a);

	void	SetEmissive(const LibMath::Vector4& color);
	void	SetEmissive(float r, float g, float b, float a);

	void	SetShininess(float shininess);

	void	SetUniforms(Shader& program, const char* variable = "objMat") const;

	// Light components
	LibMath::Vector4	m_ambient;
	LibMath::Vector4	m_diffuse;
	LibMath::Vector4	m_specular;
	LibMath::Vector4	m_emissive;

	float				m_shininess = 0.0f;
};

void ResetLights();