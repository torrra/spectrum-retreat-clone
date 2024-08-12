#pragma once

// Standard
#include <string>

// Physics library
#include "PhysicsLib/CollisionDetection.h"
#include "PhysicsLib/ColliderHierarchy.hpp"

// Engine low renderer
#include "SceneNode.hpp"
#include "Mesh.h"

// Project
#include "Color.hpp"

// Class Door inherit from ISceneObject
class Door : public ISceneObject 
{
public:
	// Constructors
	
	// Don't need to create an empty door so delete it
	Door(void) = delete;
	
	// Constructor with one parameter : color
	Door(const Color& doorColor); 
	
	// Constructor with two parameters : color, collider
	Door(const Color& doorColor, BoxBV* collider); 
	
	// Constructor with three parameters : color, collider, key
	Door(const Color& doorColor, BVHierarchy& colliderHierarchy, const std::string& key); 

	// Destructor
	~Door(void) = default;

	// Create functions
	// Create the mesh of the door
	Mesh*	CreateMesh(Graph<SceneNode>& gameObjects, const char* colorBoxKey, const char* meshKey, Model* model); 
	
	// Create the specific collider of the door
	BoxBV*	CreateCollider(BVHierarchy& colliders, const char* key, const char* parentKey = nullptr);

	// Transform
	void	Flip(bool x, bool y, bool z); // Apply a rotation of 90 degrees of the door

	// Setters
	// Get the object door
	void	SetMesh(Graph<SceneNode>& gameObjects, const char* meshKey); 

	// Get the collider of the object door
	void	SetCollider(BVHierarchy& colliders, const char* colliderKey); 

	// Update
	// Update the state of the door between two state open/close
	void	UpdateDoorState(const Color& phoneColor);

	// Variables
	static Color*	m_playerColor;
	Material		m_mat;
	Color			m_doorColor;
	BoxBV*			m_collider		= nullptr;
	Mesh*			m_mesh			= nullptr;

}; // !Class Door