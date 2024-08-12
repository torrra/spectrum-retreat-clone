#pragma once

#include <filesystem>

#include "LibMath/Vector.h"

#include "LibMath/Matrix/Matrix4.h"
#include "LibMath/Matrix/Matrix3.h"

#include "ResourceManager.h"

class Shader : public IResource
{
public:

	// Create shader program (ignore string)
	Shader(const std::string&);

	// Destroy and delete shaders
	~Shader()
	{
		Delete();
	}
	// Import both vertex and fragment shaders at once
	void InitShader(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader);

	// Read and compile vertex shader
	bool SetVertexShader(const std::filesystem::path& filename);

	// Read and compile fragment shader
	bool SetFragmentShader(const std::filesystem::path& filename);

	// Link shader program
	bool Link();

	// Use program for next draw call
	void Use();

	// Delete program and both shaders
	void Delete();


	// Set program uniform of different types

	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);

	void SetUniform(const std::string& name, const int count, const float* values);
	void SetUniform(const std::string& name, const int count, const int* values);

	void SetUniform(const std::string& name, const LibMath::Vector3& values);
	void SetUniform(const std::string& name, const LibMath::Vector4& values);

	void SetUniform(const std::string& name, const float val1, const float val2, const float val3);

	void SetUniform(const std::string& name, const LibMath::Matrix4& matrix) const;

	void SetUniform(const std::string& name, const LibMath::Matrix3& matrix) const;

private:

	// Compile vertex or fragment shader
	bool CompileShader(const std::string& source, unsigned int shaderType) const;

	// Open and read shader
	bool ImportShader(const std::filesystem::path& filename, unsigned int shaderType);

	// Program and shader IDs

	unsigned int			m_programID = 0;
	unsigned int			m_vertexID = 0;
	unsigned int			m_fragmentID = 0;
};