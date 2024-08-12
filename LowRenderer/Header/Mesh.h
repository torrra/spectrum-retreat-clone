#pragma once

#include "LibMath/Matrix/Matrix4.h"

#include "SceneNode.hpp"
#include "Model.h"
#include "Light.h"
#include "Textures.h"

class Mesh : public ISceneObject
{
public:
	// Constructor
			Mesh(Model* model);
			Mesh(Model* model, Texture* texture);

	// Destructor
			~Mesh(void) override;

	void    LinkToNode(SceneNode* node) override;

	// Mesh UI Component
	void    MeshUI(const char* meshName, int ID);
	void    SetValues(void);
	void    SetPosition(float x, float y, float z);

	// Transformation functions
	void    Translate(const LibMath::Vector3& translate);
	void    Rotate(const float& angle, const LibMath::Vector3& axis);
	void    Scale(const LibMath::Vector3& scale);

    void    SetMaterial(ResourceManager& manager, const std::string& key);

    void    CreateMaterial(void);
    void    CreateModelVAO(void);
    void    Draw(Shader& program, bool indexed = true) const;

	// Model matrix for world transformations
	// Model-View-Projection matrix

	LibMath::Vector3    m_position;
	LibMath::Vector3    m_rotation;
	LibMath::Vector3    m_prevRotation;
	LibMath::Vector3    m_scale;

	SceneNode*          m_sceneNode = nullptr;

	Model*              m_model = nullptr;
	Texture*            m_texture = nullptr;
	Material*           m_material = nullptr;
};

void AddMeshesToUI(Graph<SceneNode>& graph, std::vector<Mesh*>& meshes);

void MeshNodeUI(SceneNode* node, std::vector<Mesh*>& meshes);