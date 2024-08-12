#include "Camera.h"
#include "Mesh.h"

#include "ColorBlock.h"
#include "DoorLogic.h"
#include "Scene.h"

void ParseScene(Graph<SceneNode>& gameObjects, Shader* shader, Camera* camera, const float deltaTime)
{
	// Temporary vector to get meshes
	std::vector<Mesh*>	sceneMeshes;
	
	// Use shader
	shader->Use();

	for (SceneNode*	node : gameObjects.m_worldRoot->m_children)
		ProcessNode(node, shader, sceneMeshes, deltaTime);

	// Set uniforms
	shader->SetUniform("viewPos", camera->m_position);
	shader->SetUniform("activeOmnis", PointLight::m_active);
	shader->SetUniform("activeDirectionals", DirectionalLight::m_active);
	shader->SetUniform("activeSpots", SpotLight::m_active);

	// Draw all meshes
	for (Mesh* mesh : sceneMeshes)
		DrawMesh(mesh, shader, camera->m_viewProjection);
}

void ProcessNode(SceneNode* node, Shader* shader, std::vector<Mesh*>& meshes, const float deltaTime)
{
	// Get node type
	switch (node->m_object->m_type)
	{
	case MESH:
		// Check if visible in level (for frustum culling)
		if (node->m_render)
			// Push data into temporary mesh vector
			meshes.push_back(dynamic_cast<Mesh*>(node->m_object));
		break;
	case LIGHT:
		// Set light uniforms
		SetLight(node->m_object, shader);
		break;
	case COLOR_BLOCK:
		// Update color blocks
		UpdateColorBlock(node->m_object, deltaTime);
		break;
	case DOOR:
		// Update door collision
		UpdateDoor(node->m_object);
		break;
	default:
		break;
	}

	for (Node*	child : node->m_children)
		ProcessNode(dynamic_cast<SceneNode*>(child), shader, meshes, deltaTime);
}


void DrawMesh(Mesh* mesh, Shader* shader, const LibMath::Matrix4& viewProjection)
{
	// Check mesh contains a texture
	if (mesh->m_texture)
	{
		mesh->m_texture->BindToUnit(GL_TEXTURE0);
		shader->SetUniform("isTextured", 1);
		shader->SetUniform("meshTexture", 0);
	}
	else
		shader->SetUniform("isTextured", 0);

	// Calculate mvp & normal matrices
	LibMath::Matrix4	mvp = mesh->m_sceneNode->m_globalTransform * viewProjection;
	LibMath::Matrix4	normalMat4 = mesh->m_sceneNode->m_globalTransform.GetInverse();

	normalMat4.Transpose(normalMat4);

	// Convert matrix 4 to matrix 3 (remove translation)
	float	normalMatArray[] =
	{
		normalMat4.m_matrix[0][0], normalMat4.m_matrix[0][1], normalMat4.m_matrix[0][2],
		normalMat4.m_matrix[1][0], normalMat4.m_matrix[1][1], normalMat4.m_matrix[1][2],
		normalMat4.m_matrix[2][0], normalMat4.m_matrix[2][1], normalMat4.m_matrix[2][2]
	};

	LibMath::Matrix3	normalMat3(normalMatArray);

	// Send data to shaders
	shader->SetUniform("model", mesh->m_sceneNode->m_globalTransform);
	shader->SetUniform("mvp", mvp);
	shader->SetUniform("normalMat", normalMat3);

	// Display mesh in game
	mesh->Draw(*shader);
}


void SetLight(ISceneObject* object, Shader* shader)
{
	BaseLight*	light = dynamic_cast<BaseLight*>(object);

	// Check light is enabled & not null pointer
	if (light && light->m_enabled)
		// Set light uniforms
		light->SetUniforms(*shader);
}


void UpdateColorBlock(ISceneObject* object, const float deltaTime)
{
	// Get pointer to given color block & color block's mesh
	ColorBlock*	colorBlock = dynamic_cast<ColorBlock*>(object);
	Mesh*	mesh = colorBlock->m_mesh;

	// Update movement for relevant blocks
	colorBlock->UpdateMovement(deltaTime);

	// Re-initalize material as block color can be swapped
	mesh->m_material->InitMaterial
	(
		{ colorBlock->m_color.m_red, colorBlock->m_color.m_green, colorBlock->m_color.m_blue, 1.f },
		{ colorBlock->m_color.m_red, colorBlock->m_color.m_green, colorBlock->m_color.m_blue, 1.f },
		{ colorBlock->m_color.m_red, colorBlock->m_color.m_green, colorBlock->m_color.m_blue, 1.f },
		32.f
	);

	// Check color block has light
	if (colorBlock->m_light)
	{
		// Update position of color block mesh
		LibMath::Vector3	meshPosition =
		{
			mesh->m_sceneNode->m_globalTransform.m_matrix[3][0],
			mesh->m_sceneNode->m_globalTransform.m_matrix[3][1],
			mesh->m_sceneNode->m_globalTransform.m_matrix[3][2]
		};

		// Re-intialize point light with updated position & color
		colorBlock->m_light->InitPointLight(meshPosition, colorBlock->m_color);
		colorBlock->m_light->SetAttenuation({ 1.0f, 0.22f, 0.20f });
	}
}


void UpdateDoor(ISceneObject* object)
{
	Door*	door = dynamic_cast<Door*>(object);

	// Check phone color is not null pointer
	if (Door::m_playerColor)
		door->UpdateDoorState(*Door::m_playerColor);
}
