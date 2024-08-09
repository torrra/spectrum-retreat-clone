// Game header
#include "DoorLogic.h"

Color* Door::m_playerColor = nullptr;

/*************\
* Constructor *
\*************/

// Construct the door with a color
Door::Door(const Color& doorColor)
{
	m_type		= DOOR;
	m_doorColor	= doorColor;
}

// Construct the door with a color and a collider
Door::Door(const Color& doorColor, BoxBV* collider)
{
	m_collider	= collider;
	m_doorColor	= doorColor;
	m_type		= DOOR;
}

// Construct the door with a color, a collider and a key
Door::Door(const Color& doorColor, BVHierarchy& colliderHierarchy, const std::string& key)
{
	m_collider	= colliderHierarchy.AddCollider<BoxBV>(key);
	m_doorColor	= doorColor;
	m_type		= DOOR;
}

/********\
* Create *
\********/

// Create the door collider
BoxBV* Door::CreateCollider(BVHierarchy& colliders, const char* key, const char* parentKey)
{
	// If there is a parent so add a collider in regard to the parent
	if (parentKey)
		m_collider = colliders.AddCollider<BoxBV>(std::string(parentKey), std::string(key), m_mesh->m_position, m_mesh->m_scale);
	
	// If there is no parent add a collider without parent
	else
		m_collider = colliders.AddCollider<BoxBV>(std::string(key), m_mesh->m_position, m_mesh->m_scale);

	return m_collider;
}

// Create the door mesh
Mesh* Door::CreateMesh(Graph<SceneNode>& gameObjects, const char* doorKey, const char* meshKey, Model* model)
{
	// Add child to the graph of mesh type
	m_mesh = gameObjects.AddChild<Mesh>(std::string(doorKey), std::string(meshKey), model);

	// Link the mesh to the scene node
	m_mesh->LinkToNode(gameObjects.GetNode(meshKey)); 

	// Give a material to the door
	m_mesh->m_material = &m_mat; 

	return m_mesh;
}

/***********\
* Transform *
\***********/

// To rotate the door
void Door::Flip(bool x, bool y, bool z)
{
	// Rotate on the x axis
	if (x) 
		m_mesh->Rotate(90.f, { 1.f, 0.f, 0.f });

	// Rotate on the y axis
	if (y) 
		m_mesh->Rotate(80.f, { 0.f, 1.f, 0.f });

	// Rotate on the z axis
	if (z) 
		m_mesh->Rotate(90.f, { 0.f, 0.f, 1.f });
}

/********\
* Setter *
\********/

// Get the object mesh
void Door::SetMesh(Graph<SceneNode>& gameObjects, const char* meshKey)
{
	m_mesh = GetObject<Mesh>(gameObjects, meshKey);
}

// Get the object collider
void Door::SetCollider(BVHierarchy& colliders, const char* colliderKey)
{
	m_collider = GetObject<BoxBV>(colliders.m_hierarchy, colliderKey);
}

/********\
* Update *
\********/

// Update the state of the door between open and close state
void Door::UpdateDoorState(const Color& phoneColor)
{
	/*If the color of the phone is note the same of the door
	  So collider is enabled*/
	m_collider->m_enabled = (m_doorColor != phoneColor);

	if (!m_collider->m_enabled)
	{
		// Set the material opacity more transparent
		m_mesh->m_material->InitMaterial
		(
			{ m_doorColor.m_red, m_doorColor.m_green, m_doorColor.m_blue, 0.2f },
			{ m_doorColor.m_red, m_doorColor.m_green, m_doorColor.m_blue, 0.2f },
			{ m_doorColor.m_red, m_doorColor.m_green, m_doorColor.m_blue, 0.2f },
			{ m_doorColor.m_red * 0.7f, m_doorColor.m_green * 0.7f, m_doorColor.m_blue * 0.7f, 0.2f },
			32.f
		);
	}
	else
	{
		// Set the material opacity less transparent
		m_mesh->m_material->InitMaterial
		(
			{ m_doorColor.m_red, m_doorColor.m_green, m_doorColor.m_blue, 0.5f },
			{ m_doorColor.m_red, m_doorColor.m_green, m_doorColor.m_blue, 0.5f },
			{ m_doorColor.m_red, m_doorColor.m_green, m_doorColor.m_blue, 0.5f },
			{ m_doorColor.m_red * 0.7f, m_doorColor.m_green * 0.7f, m_doorColor.m_blue * 0.7f, 0.5f },
			32.f
		);
	}
}



