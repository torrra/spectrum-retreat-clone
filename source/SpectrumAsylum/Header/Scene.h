#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Graph.hpp"
#include "SceneNode.hpp"

// Display mesh in level
void	ParseScene(Graph<SceneNode>& gameObjects, Shader* shader, Camera* camera, const float deltaTime);

// Update nodes
void	ProcessNode(SceneNode* node, Shader* shader, std::vector<Mesh*>& meshes, const float deltaTime);

// Set uniforms for mesh
void	DrawMesh(Mesh* mesh, Shader* shader, const LibMath::Matrix4& viewProjection);

// Set light uniforms
void	SetLight(ISceneObject* object, Shader* shader);

// Update color blocks
void	UpdateColorBlock(ISceneObject* object, const float deltaTime);

// Update door
void	UpdateDoor(ISceneObject* object);