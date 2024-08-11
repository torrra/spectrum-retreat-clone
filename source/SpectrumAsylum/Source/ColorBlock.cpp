// Standard library
#include <string>

// Game Headers
#include "ColorBlock.h"

/*************\
* CONSTRUCTOR *
\*************/

ColorBlock::ColorBlock(void)
{
	m_type = COLOR_BLOCK;
}


ColorBlock::ColorBlock(const Color& color, Mesh* mesh, ColBoxBV* collider)
	: m_color(color), m_mesh(mesh), m_collider(collider)
{
	m_type = COLOR_BLOCK;
}


ColorBlock::ColorBlock(const Color& color, Mesh* mesh)
	: m_color(color), m_mesh(mesh)
{
	m_type = COLOR_BLOCK;
}


ColorBlock::ColorBlock(const Color& color)
	: m_color(color)
{
	m_type = COLOR_BLOCK;
}


/************\
* DESTRUCTOR *
\************/

ColorBlock::~ColorBlock(void)
{
	if (m_movement)
		delete m_movement;
}


/********\
* Create *
\********/

// Create the collider of cube
ColBoxBV* ColorBlock::CreateCollider(BVHierarchy& colliders, const char* key, const char* parentKey)
{
	// If collider of this key already exists then return nullptr
	if (m_collider)
	{

#ifndef NDEBUG
		std::cout << "Collider already exists for this object\n";
#endif
		return nullptr;
	}

	// If there is a parent then create a collider in regard to the parent
	if (parentKey)
		m_collider = colliders.AddCollider<ColBoxBV>(std::string(parentKey), std::string(key), m_mesh->m_position, m_mesh->m_scale);
	// If there is no parent, then create a collider without parent
	else
		m_collider = colliders.AddCollider<ColBoxBV>(std::string(key), m_mesh->m_position, m_mesh->m_scale);

	// Apply the color of the block to the collider
	m_collider->m_color = &m_color;

	return m_collider;

}


// Create the mesh of the cube
Mesh* ColorBlock::CreateMesh(Graph<SceneNode>& gameObjects, const char* colorBoxKey, const char* meshKey, Model* model)
{
	// If the cube already exist then there is already a mesh
	if (m_mesh)
	{
#ifndef NDEBUG
		std::cout << "Mesh already exists for this object\n";
#endif

		// So return nullptr
		return nullptr;
	}

	// Add the child to the graph scene
	m_mesh = gameObjects.AddChild<Mesh>(std::string(colorBoxKey), std::string(meshKey), model);

	// Link the mesh to the scene node
	m_mesh->LinkToNode(gameObjects.GetNode(meshKey));

	// Give him a material
	m_mesh->m_material = &m_mat;

	return m_mesh;
}


/********\
* Setter *
\********/

// Set the cube mesh
void ColorBlock::SetMesh(Graph<SceneNode>& gameObjects, const char* meshKey)
{
	// Get the cube mesh
	m_mesh = GetObject<Mesh>(gameObjects, meshKey);
}


// Set the collider
void ColorBlock::SetCollider(BVHierarchy& colliders, const char* colliderKey)
{
	// Get the collider
	m_collider = GetObject<ColBoxBV>(colliders.m_hierarchy, colliderKey);
}


// Set the light
void ColorBlock::SetLight(Graph<SceneNode>& gameObjects, const char* lightKey)
{
	// Get the light
	m_light = GetObject<PointLight>(gameObjects, std::string(lightKey));
}


// Set the position of a cube
void ColorBlock::SetPosition(const LibMath::Vector3& pos)
{
	// Error if the mesh does not exist
	if (!m_mesh)
	{
#ifndef NDEBUG
		std::cout << "Unable to set a position without an existing mesh\n";
#endif
		return;
	}

	// Use mesh translate function
	m_mesh->Translate(pos);
}


// Set the movement of a cube
void ColorBlock::SetMovement(float speed, float length)
{
	// If movement of a block exist
	if (m_movement)
	{
		// Then set the speed
		m_movement->m_speed = speed;

		// Then set the length of the movement
		m_movement->m_pathLength = length;
	}
	// If mesh exist but movement not initialize
	else if (m_mesh)
	{
		// Then create a movement
		m_movement = new BlockMovement(speed, length, m_mesh);
	}
}


// Set movement with direction
void ColorBlock::SetMovement(float speed, float length, BLOCK_DIRECTION directions[NUM_OF_DIR])
{
	// If movement of a block exist
	if (m_movement)
	{
		// Then set the speed
		m_movement->m_speed = speed;
		// Then set the length of the movement
		m_movement->m_pathLength = length;

		// For each different direction
		for (int	index = 0; index < NUM_OF_DIR; ++index)
		{
			// give to movement the direction indicated
			m_movement->m_order[index] = directions[index];
		}
	}

	// If mesh exist but movement not initialize
	else if (m_mesh)
	{
		// Then create a movement
		m_movement = new BlockMovement(speed, length, directions, m_mesh);
	}
}


// Set movement with two directions
void ColorBlock::SetMovement(float speed, float length, BLOCK_DIRECTION dir1, BLOCK_DIRECTION dir2)
{
	// If movement of a block exist
	if (m_movement)
	{
		// Create an array of two enum to set the two directions
		BLOCK_DIRECTION	directions[2] = { dir1, dir2 };

		// For each different direction
		for (int	index = 0; index < NUM_OF_DIR; ++index)
		{
			// give to movement the direction indicated
			m_movement->m_order[index] = directions[index % 2];
		}

		// Then set the speed
		m_movement->m_speed = speed;
		// Then set the length of the movement
		m_movement->m_pathLength = length;
	}
	// If mesh exist but movement not initialize
	else if (m_mesh)
	{
		// Then create a movement
		m_movement = new BlockMovement(speed, length, dir1, dir2, m_mesh);
	}
}

/********\
* Update *
\********/

// Update the movement of the cube using a switch case
void ColorBlock::UpdateMovement(const float deltaTime)
{
	// Break if the movement or the mesh does not exists
	if (!m_movement || !m_mesh)
		return;

	// Create a matrix 4 to apply transformation of mesh
	LibMath::Matrix4&	meshTransform = m_mesh->m_sceneNode->m_globalTransform;

	// Switch between the different direction to apply different transformation
	switch (m_movement->m_order[m_movement->m_currentDir])
	{
	// For left and right use this matrix
	case LEFT:
	case RIGHT:
		m_movement->UpdateAxis(meshTransform.m_matrix[3][0], deltaTime);
		break;

	// For up and down use this matrix
	case UP:
	case DOWN:
		m_movement->UpdateAxis(meshTransform.m_matrix[3][1], deltaTime);
		break;

	// For forward and backward use this matrix
	case FORWARD:
	case BACKWARD:
		m_movement->UpdateAxis(meshTransform.m_matrix[3][2], deltaTime);
		break;
	}
}


/*********\
* Boolean *
\*********/
// Check if the cube is touch with the ray of the ray cast
bool ColorBlock::IsHit(const Ray& ray, Color& playerColor)
{
	float	distance;

	// Check the collision
	bool	collision = ray.Intersect(*m_collider, distance);

	// If there is collision
	if (collision)
		// Then change the color of the cube
		m_color.Swap(playerColor);

	return collision;
}