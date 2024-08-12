#pragma once

// Engine LowRenderer
#include "SceneNode.hpp"
#include "Mesh.h"

// LibMath
#include "LibMath/Arithmetic.h"

// Physics library
#include "PhysicsLib/CollisionDetection.h"
#include "PhysicsLib/RayCast.h"

// Project
#include "Color.hpp"

// Define for the number of directions (see the enumerator)
#define NUM_OF_DIR	6 

// ColorBlock inherit of ISceneObject
class ColorBlock : public ISceneObject 
{
public:
	// Block direction enumerator
	enum BLOCK_DIRECTION
	{
		FORWARD,
		BACKWARD,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};


	/// Forward class declaration
	class BlockMovement;

	// Constructor
	ColorBlock(void);
	ColorBlock(const Color& color, Mesh* mesh, ColBoxBV* collider);
	ColorBlock(const Color& color, Mesh* mesh);
	ColorBlock(const Color& color);

	// Destructor
	~ColorBlock(void);

	// Create functions
	ColBoxBV*	CreateCollider(BVHierarchy& colliders, const char* key, const char* parentKey = nullptr);
	Mesh*	CreateMesh(Graph<SceneNode>& gameObjects, const char* colorBoxKey, const char* meshKey, Model* model);

	// Setter
	void	SetMesh(Graph<SceneNode>& gameObjects, const char* meshKey);
	void	SetCollider(BVHierarchy& colliders, const char* colliderKey);
	void	SetLight(Graph<SceneNode>& gameObjects, const char* lightKey);
	void	SetPosition(const LibMath::Vector3& pos);
	void	SetMovement(float speed, float length);
	void	SetMovement(float speed, float length, BLOCK_DIRECTION directions[NUM_OF_DIR]);
	void	SetMovement(float speed, float length, BLOCK_DIRECTION dir1, BLOCK_DIRECTION dir2);

	// Update
	void	UpdateMovement(const float deltaTime);

	// Boolean
	bool	IsHit(const Ray& ray, Color& playerColor);

	// Variables
	Material		m_mat;
	Color			m_color;

	Mesh*			m_mesh		= nullptr;
	ColBoxBV*		m_collider	= nullptr;
	PointLight*		m_light		= nullptr;

private:
	BlockMovement*	m_movement	= nullptr;

}; // !Class ColorBlock

// Nested class BlockMovement of ColorBlock class
class ColorBlock::BlockMovement
{
private:

	/*
	* Constructors
	* Don't want to initialize a movement = 0 so delete it 
	*/
	BlockMovement(void) = delete; 

	// Don't want to copy movement so delete it
	BlockMovement(const BlockMovement&) = delete; 

	BlockMovement(float speed, float length, Mesh* mesh)
		: m_speed(speed), m_pathLength(length), m_blockMesh(mesh)
	{
		// Use SetStarPoint function to define the start point of the cube movement
		SetStartPoint(); 
	}

	BlockMovement(float speed, float length, BLOCK_DIRECTION directions[NUM_OF_DIR], Mesh* mesh)
		: m_speed(speed), m_pathLength(length), m_blockMesh(mesh)
	{
		// For each different direction
		for (int	index = 0; index < NUM_OF_DIR; ++index)
		{
			// Give to movement the direction indicated
			m_order[index] = directions[index];
		}

		// Use SetStarPoint function to define the start point of the cube movement
		SetStartPoint(); 
	}

	BlockMovement(float speed, float length, BLOCK_DIRECTION dir1, BLOCK_DIRECTION dir2, Mesh* mesh)
		: m_speed(speed), m_pathLength(length), m_blockMesh(mesh)
	{
		// Create a movement with two different directions
		BLOCK_DIRECTION	directions[2] = { dir1, dir2 };

		for (int	index = 0; index < NUM_OF_DIR; ++index)
		{
			// give to movement the direction indicated
			m_order[index] = directions[index % 2];
		}

		// Use SetStarPoint function to define the start point of the cube movement
		SetStartPoint();
	}

	// Destructor
	~BlockMovement(void) = default;

	// Update
	void UpdateAxis(float& pos, const float deltaTime)
	{
		float	distance = LibMath::absolute(pos - m_startPoint);

		/*Check the distance the cube course and call update if the distance
		  is > to the pathLength to change it direction*/
		if (distance >= m_pathLength)
			UpdateDirection();

		// Apply speed to the cube
		else
			pos += m_speed * deltaTime;
	}

	void UpdateDirection(void)
	{
		// Set a counter
		if (m_currentDir < 5)
			++m_currentDir;
		else
			m_currentDir = 0;

		// If the counter is = to 0
		if (m_currentDir == 0) 
			// Use SetStarPoint function to define the start point of the cube movement
			SetStartPoint(); 
		else
			// Call the update of the start point
			UpdateStartPoint(); 
	}

	void UpdateStartPoint(void)
	{
		// Create a matrix for the transformation of the mesh
		LibMath::Matrix4& meshTransform = m_blockMesh->m_sceneNode->m_globalTransform; 

		// Switch between the different directions
		switch (m_order[m_currentDir]) 
		{
		case RIGHT:
			m_startPoint = meshTransform.m_matrix[3][0];
			m_speed = LibMath::absolute(m_speed);
			break;

		case UP:
			m_startPoint = meshTransform.m_matrix[3][1];
			m_speed = LibMath::absolute(m_speed);
			break;

		case FORWARD:
			m_startPoint = meshTransform.m_matrix[3][2];
			m_speed = LibMath::absolute(m_speed);
			break;

		case LEFT:
			m_startPoint = meshTransform.m_matrix[3][0];
			m_speed = LibMath::absolute(m_speed) * -1;
			break;

		case DOWN:
			m_startPoint = meshTransform.m_matrix[3][1];
			m_speed = LibMath::absolute(m_speed) * -1;
			break;

		case BACKWARD:
			m_startPoint = meshTransform.m_matrix[3][2];
			m_speed = LibMath::absolute(m_speed) * -1;
			break;

		default: 
			break;
		}
	}

	// Setter
	void SetStartPoint(void)
	{
		// Switch between different direction to set the start point
		switch (m_order[m_currentDir]) 
		{
		case RIGHT:
			m_startPoint = m_blockMesh->m_position.m_x;
			m_speed = LibMath::absolute(m_speed);
			break;

		case UP:
			m_startPoint = m_blockMesh->m_position.m_y;
			m_speed = LibMath::absolute(m_speed);
			break;

		case FORWARD:
			m_startPoint = m_blockMesh->m_position.m_z;
			m_speed = LibMath::absolute(m_speed);
			break;

		case LEFT:
			m_startPoint = m_blockMesh->m_position.m_x;
			m_speed = LibMath::absolute(m_speed) * -1;
			break;

		case DOWN:
			m_startPoint = m_blockMesh->m_position.m_y;
			m_speed = LibMath::absolute(m_speed) * -1;
			break;

		case BACKWARD:
			m_startPoint = m_blockMesh->m_position.m_z;
			m_speed = LibMath::absolute(m_speed) * -1;
			break;

		default: 
			break;
		}
	}

	// Variables
	BLOCK_DIRECTION	m_order[NUM_OF_DIR] = { LEFT, RIGHT, LEFT, RIGHT, LEFT, RIGHT };
	Mesh*			m_blockMesh			= nullptr;
	float			m_speed				= 12.f;
	float			m_pathLength		= 1.f;
	float			m_startPoint		= 0.f;
	int				m_currentDir		= 0;

	friend class ColorBlock;
};