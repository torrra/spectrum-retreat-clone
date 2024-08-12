#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "LibMath/Arithmetic.h"

#include "Camera.h"
#include "Mesh.h"

// Create a mesh with only a model
Mesh::Mesh(Model* model)
	: m_model(model)
{
	m_position = {0.0f, 0.0f, 0.0f};
	m_rotation = {0.0f, 0.0f, 0.0f};
	m_scale = {1.0f, 1.0f, 1.0f};
	m_type = MESH;
	m_texture = model->m_texture;
}

// Create a mesh with a model and a texture
Mesh::Mesh(Model * model, Texture * texture)
	: m_model(model), m_texture(texture)
{
	m_position = {0.0f, 0.0f, 0.0f};
	m_rotation = {0.0f, 0.0f, 0.0f};
	m_scale = {1.0f, 1.0f, 1.0f};
	m_type = MESH;
}

Mesh::~Mesh()
{
}

void Mesh::LinkToNode(SceneNode* node)
{
	m_sceneNode = node;
}

void Mesh::SetPosition(float x, float y, float z)
{
	this->m_position = { x, y, z };
}

void Mesh::Translate(const LibMath::Vector3& translate)
{
	// Apply translation to mesh

	if (m_sceneNode)
	{
		// Initialize a 4x4 matrix to all zeros
		LibMath::Matrix4 translationMatrix;

		// Convert to identity matrix
		translationMatrix.m_matrix[0][0] = 1.0f;
		translationMatrix.m_matrix[1][1] = 1.0f;
		translationMatrix.m_matrix[2][2] = 1.0f;

		translationMatrix.m_matrix[3][0] = translate[0];
		translationMatrix.m_matrix[3][1] = translate[1];
		translationMatrix.m_matrix[3][2] = translate[2];
		translationMatrix.m_matrix[3][3] = 1.f;

		// Apply translation
		m_sceneNode->m_localTransform = translationMatrix * m_sceneNode->m_localTransform;
		m_sceneNode->m_dirty = true;
	}

	m_position += translate;
}

void Mesh::Rotate(const float& angle, const LibMath::Vector3& axis)
{
	// Function to apply a rotation by a given angle (in radians) around an arbitrary axis

	if (m_sceneNode)
	{

		// Initialize 4x4 matrix to all zeros
		LibMath::Matrix4			rotationMatrix;

		// Store return values as variables in order to prevent multiple function calls
		const LibMath::Vector3		normalizedAxis = axis.normalizedCopy();
		const float					oneMinCos = 1.0f - cosf(angle);
		const float					cosAngle = cosf(angle);
		const float					sinAngle = sinf(angle);

		// Rotation matrix line one
		rotationMatrix.m_matrix[0][0] = cosAngle + LibMath::power(normalizedAxis.m_x, 2) * oneMinCos;
		rotationMatrix.m_matrix[1][0] = normalizedAxis.m_x * normalizedAxis.m_y * oneMinCos - normalizedAxis.m_z * sinAngle;
		rotationMatrix.m_matrix[2][0] = normalizedAxis.m_x * normalizedAxis.m_z * oneMinCos + normalizedAxis.m_y * sinAngle;

		// Rotation matrix line two
		rotationMatrix.m_matrix[0][1] = normalizedAxis.m_y * normalizedAxis.m_x * oneMinCos + normalizedAxis.m_z * sinAngle;
		rotationMatrix.m_matrix[1][1] = cosAngle + LibMath::power(normalizedAxis.m_y, 2) * oneMinCos;
		rotationMatrix.m_matrix[2][1] = normalizedAxis.m_y * normalizedAxis.m_z * oneMinCos - normalizedAxis.m_x * sinAngle;

		// Rotation matrix line three
		rotationMatrix.m_matrix[0][2] = normalizedAxis.m_z * normalizedAxis.m_x * oneMinCos - normalizedAxis.m_y * sinAngle;
		rotationMatrix.m_matrix[1][2] = normalizedAxis.m_z * normalizedAxis.m_y * oneMinCos + normalizedAxis.m_x * sinAngle;
		rotationMatrix.m_matrix[2][2] = cosAngle + LibMath::power(normalizedAxis.m_z, 2) * oneMinCos;

		// Rotation matrix line four
		rotationMatrix.m_matrix[3][3] = 1.0f;


		m_sceneNode->m_localTransform = rotationMatrix * m_sceneNode->m_localTransform;

		m_sceneNode->m_dirty = true;
	}

}

void Mesh::Scale(const LibMath::Vector3& scale)
{
	// Apply a scale transformation
	if (m_sceneNode)
	{
		// Initialize 4x4 matrix
		LibMath::Matrix4 scalingMatrix;

		// Initialize matrix value to given scale x, y, z components
		scalingMatrix.m_matrix[0][0] = scale[0];
		scalingMatrix.m_matrix[1][1] = scale[1];
		scalingMatrix.m_matrix[2][2] = scale[2];
		scalingMatrix.m_matrix[3][3] = 1.f;

		// Apply translation

		m_sceneNode->m_localTransform = scalingMatrix * m_sceneNode->m_localTransform;

		m_sceneNode->m_dirty = true;
	}

	m_scale = scale;

}

void Mesh::CreateMaterial()
{
	// Check material does not already exist, if not create a new material
	if (!m_material)
		m_material = new Material();
}

void Mesh::SetMaterial(ResourceManager& manager, const std::string& key)
{
	m_material = manager.Get<Material>(key);
}

void Mesh::Draw(Shader& program, bool indexed) const
{
	/*
	*	Assign values for all the uniform material variables
	*	in the shader if the given mesh has a material.
	*/
	if (m_material)
		m_material->SetUniforms(program);

	// Check if the model is not a null pointer & if we are using an EBO
	if (m_model && indexed)
		m_model->m_vao.DrawIndexed();

	// Check if model is not a null pointer, if yes draw without the EBO
	else if (m_model)
		m_model->m_vao.DrawUnindexed();
}

void Mesh::CreateModelVAO()
{
	// Create a new vertex array object if the model is not a null pointer
	if (m_model)
		m_model->CreateVAO();
}


void AddMeshesToUI(Graph<SceneNode>& graph, std::vector<Mesh*>& meshes)
{
	for (SceneNode* node : graph.m_worldRoot->m_children)
		MeshNodeUI(node, meshes);
}

void MeshNodeUI(SceneNode* node, std::vector<Mesh*>& meshes)
{
	if (node->m_object->m_type == MESH)
	{
		Mesh* mesh = dynamic_cast<Mesh*>(node->m_object);

		if (!mesh)
			return;

		meshes.push_back(mesh);
	}

	for (Node* child : node->m_children)
	{
		SceneNode* sceneChild = dynamic_cast<SceneNode*>(child);

		if (!sceneChild)
			return;

		MeshNodeUI(sceneChild, meshes);
	}
}