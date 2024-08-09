#include <fstream>
#include <filesystem>
#include <iostream>

#include "glad/glad.h"

#include "Shader.h"

Shader::Shader(const std::string&)
{
	// Create OpenGL shader program
	m_programID = glCreateProgram();
}

void Shader::InitShader(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader)
{
	bool success = this->SetVertexShader(vertexShader);

	if (!success)
		__debugbreak();
	else
		this->SetFragmentShader(fragmentShader);

	this->Link();
}

bool Shader::SetVertexShader(const std::filesystem::path& filename)
{
	// Add shader subfolder to path
	std::filesystem::path relativePath("shaders/");

	relativePath += filename;

	// Open file from updated path
	return ImportShader(relativePath, GL_VERTEX_SHADER);
}


bool Shader::SetFragmentShader(const std::filesystem::path& filename)
{
	// Add shader subfolder to path
	std::filesystem::path relativePath("shaders/");

	relativePath += filename;

	// Open file from updated path
	return ImportShader(relativePath, GL_FRAGMENT_SHADER);
}

bool Shader::CompileShader(const std::string& source, unsigned int shaderType) const
{
	// Compilation status
	int success;

	// Shader ID depending on which shader is being compiled
	unsigned int id = (shaderType == GL_VERTEX_SHADER) ? m_vertexID : m_fragmentID;


	// Get source code as C-style string
	const char* sourceCstr = source.c_str();

	// Pass source code to OpenGL
	glShaderSource(id, 1, &sourceCstr, NULL);

	// Compile shader
	glCompileShader(id);

	// Get compilation status
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	// Get error if compilation fails
	if (!success)
	{
		// Store error message
		char infoLog[512];

		// Get error message
		glGetShaderInfoLog(id, 512, NULL, infoLog);

		// Display error message

		if (shaderType == GL_FRAGMENT_SHADER)
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		else if (shaderType == GL_VERTEX_SHADER)
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;

	}

	return true;
}

bool Shader::ImportShader(const std::filesystem::path& filename, unsigned int shaderType)
{
	// Open specified file
	std::ifstream  sourceFile(filename, std::ios::in | std::ios::binary);

	// Read file if successfully opened
	if (sourceFile)
	{
		// Store source code
		char* sourceCode;

		// Go to end of file
		sourceFile.seekg(0, std::ios::end);

		// Store last index (starting from 1) as file length
		unsigned int length = static_cast<unsigned int>(sourceFile.tellg());

		// Allocate memory for source code
		sourceCode = new char[length + 1];

		// Go back to begining of file
		sourceFile.seekg(std::ios::beg);

		// Read entire file
		sourceFile.read(sourceCode, length);

		// Add null terminator to code string
		sourceCode[length] = '\0';

		// Close file
		sourceFile.close();


		// Create OpenGL shader

		if (shaderType == GL_VERTEX_SHADER)
			m_vertexID = glCreateShader(shaderType);
		else
			m_fragmentID = glCreateShader(shaderType);

		// Compile shader
		bool compileStatus = CompileShader(sourceCode, shaderType);

		// Delete compiled string
		delete[] sourceCode;

		return compileStatus;
	}

	// Tell user file could not be opened if
	std::cout << "ERROR::SHADER::CANNOT_OPEN_FILE\n" << filename << std::endl;
	return false;
}

bool Shader::Link()
{
	// Attach both shaders to progran
	glAttachShader(m_programID, m_vertexID);
	glAttachShader(m_programID, m_fragmentID);

	// Link status
	int success;

	// Link program
	glLinkProgram(m_programID);

	// Get link status
	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

	// Get error if linking fails
	if (!success)
	{
		// Error message
		char infoLog[512];

		// Get error message
		glGetProgramInfoLog(m_programID, 512, NULL, infoLog);

		// Display error message
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	return true;
}

void Shader::Use()
{
	// Use OpenGL program
	glUseProgram(m_programID);
}

void Shader::SetUniform(const std::string& name, int value)
{
	// Set int uniform
	glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::SetUniform(const std::string& name, float value)
{
	// Set float uniform
	glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::SetUniform(const std::string& name, const LibMath::Matrix4& matrix) const
{
	// Set Matrix4 uniform
	glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &matrix.m_matrix[0][0]);
}

void Shader::SetUniform(const std::string& name, const LibMath::Matrix3& matrix) const
{
	// Set Matrix4 uniform
	glUniformMatrix3fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, &matrix.m_matrix[0][0]);
}

void Shader::Delete()
{
	// Delete both shaders and program

	if (m_vertexID)
		glDeleteShader(m_vertexID);
	if (m_fragmentID)
		glDeleteShader(m_fragmentID);

	if (m_programID)
		glDeleteProgram(m_programID);

	m_vertexID = 0;
	m_fragmentID = 0;
	m_programID = 0;
}

void Shader::SetUniform(const std::string& name, const int count, const float* values)
{
	// Set several sets of 3 floats
	glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), count, values);

}

void Shader::SetUniform(const std::string& name, const int count, const int* values)
{
	// Set several sets of 3 ints
	glUniform3iv(glGetUniformLocation(m_programID, name.c_str()), count, values);

}

void Shader::SetUniform(const std::string& name, const LibMath::Vector3& values)
{
	// Set 3 floats as uniforms
	glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, &values.m_x);
}

void Shader::SetUniform(const std::string& name, const LibMath::Vector4& values)
{
	// Set 4 floats as uniforms
	glUniform4fv(glGetUniformLocation(m_programID, name.c_str()), 1, &values.m_x);

}

void Shader::SetUniform(const std::string& name, const float val1, const float val2, const float val3)
{
	// Set 3 floats as uniforms
	glUniform3f(glGetUniformLocation(m_programID, name.c_str()), val1, val2, val3);

}