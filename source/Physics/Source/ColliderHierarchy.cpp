#include "PhysicsLib/ColliderHierarchy.hpp"


BVHierarchy::BVNode::BVNode(BVNode* parent, Collider* collider)
	: m_parent(parent), m_collider(collider)
{}

void BVHierarchy::BVNode::Update(const Frustum & frustum)
{
	// Update node according to type with frustum culling
	switch (m_collider->m_type)
	{
	case PhysicsLib::BOX:
	case PhysicsLib::COLOR_CUBE:
		UpdateBox(frustum);
		break;

	case PhysicsLib::SPHERE:
		UpdateSphere();
		break;

	default: break;
	}

	// Update children
	for (BVNode* child : m_children)
	{
		child->Update(frustum);
	}

}

void BVHierarchy::BVNode::Update()
{
	// Update node according to type WITHOUT frustum culling
	switch (m_collider->m_type)
	{
	case PhysicsLib::BOX:
	case PhysicsLib::COLOR_CUBE:
		UpdateBox();
		break;

	case PhysicsLib::SPHERE:
		UpdateSphere();
		break;

	default: break;
	}

	// Update children
	for (BVNode* child : m_children)
	{
		child->Update();
	}
}


void BVHierarchy::BVNode::UpdateBox()
{
	// Get AABB
	BoxBV*		box = dynamic_cast<BoxBV*>(m_collider);

	// Update collider position and min/max vertices if it has a scene node
	if (m_sceneNode)
	{
		// Get translation and scale from scene node matrix

		box->m_position =
		{
			m_sceneNode->m_globalTransform.m_matrix[3][0],
			m_sceneNode->m_globalTransform.m_matrix[3][1],
			m_sceneNode->m_globalTransform.m_matrix[3][2]
		};

		box->m_boxScale =
		{
			m_sceneNode->m_globalTransform.m_matrix[0][0],
			m_sceneNode->m_globalTransform.m_matrix[1][1],
			m_sceneNode->m_globalTransform.m_matrix[2][2]
		};
	}

	// Update min and max vertices
	box->m_minVertex = box->m_position - box->m_boxScale;
	box->m_maxVertex = box->m_position + box->m_boxScale;
}

void BVHierarchy::BVNode::UpdateBox(const Frustum& frustum)
{
	// Get AABB
	BoxBV*		box = dynamic_cast<BoxBV*>(m_collider);

	// Update collider position and min/max vertices if it has a scene node
	if (m_sceneNode)
	{
		// Only render mesh if inside view frustum
		if (!frustum.Intersect(*box))
			m_sceneNode->m_render = false;
		else
			m_sceneNode->m_render = true;

		// Get translation and scale from scene node matrix

		box->m_position =
		{
			m_sceneNode->m_globalTransform.m_matrix[3][0],
			m_sceneNode->m_globalTransform.m_matrix[3][1],
			m_sceneNode->m_globalTransform.m_matrix[3][2]
		};

		box->m_boxScale =
		{
			m_sceneNode->m_globalTransform.m_matrix[0][0],
			m_sceneNode->m_globalTransform.m_matrix[1][1],
			m_sceneNode->m_globalTransform.m_matrix[2][2]
		};
	}

	// Update min and max vertices
	box->m_minVertex = box->m_position - box->m_boxScale;
	box->m_maxVertex = box->m_position + box->m_boxScale;
}


void BVHierarchy::BVNode::UpdateSphere()
{
	if (!m_sceneNode)
		return;

	// Get sphere
	SphereBV*	sphere = dynamic_cast<SphereBV*>(m_collider);

	// Update sphere from scene node matrix
	sphere->m_position =
	{
		m_sceneNode->m_globalTransform.m_matrix[3][0],
		m_sceneNode->m_globalTransform.m_matrix[3][1],
		m_sceneNode->m_globalTransform.m_matrix[3][2]
	};
}


bool BVHierarchy::BVNode::TestCollision(Collider* target)
{
	// Create variables to perform appropriate tests
	BoxBV*			boxTarget = nullptr;
	SphereBV*		sphereTarget = nullptr;

	switch (target->m_type)
	{
	// Cuboid types
	case PhysicsLib::LIGHT_BOX:
	case PhysicsLib::DOOR:
	case PhysicsLib::BOX:
	case PhysicsLib::COLOR_CUBE:
	case PhysicsLib::HOLED:
	case PhysicsLib::TELEPORTER:
	case PhysicsLib::FINAL_TOWER:
		boxTarget = dynamic_cast<BoxBV*>(target);

		return CheckNodeType(boxTarget);

	// Sphere
	case PhysicsLib::SPHERE:
		sphereTarget = dynamic_cast<SphereBV*>(target);

		return CheckNodeType(sphereTarget);

	// Invalid/irrelevant type
	default:
		return false;

	}

}

void BVHierarchy::BVNode::PruneColliders(Collider* target, std::vector<BVHierarchy::BVNode*>& prunedList, BVHierarchy::BVNode* collider)
{
	// Prune collider's children if no intersection occured
	if (!collider->TestCollision(target))
	{
		return;
	}

	// Add its cildren otherwise
	if (collider->m_children.size())
	{
		for (BVHierarchy::BVNode* child : collider->m_children)
		{
			if (child->m_children.size())
				PruneColliders(target, prunedList, child);
			else
				prunedList.push_back(child);
		}
	}

}

BVHierarchy::BVNode::~BVNode(void)
{
	// Delete collider
	if (m_collider)
	{
		delete m_collider;
		m_collider = nullptr;
	}

	// Delete children nodes
	for (BVNode* child : m_children)
	{
		if (child)
		{
			child->~BVNode();
			delete child;
		}
	}

	m_children.~vector();
}

bool BVHierarchy::BVNode::CheckNodeType(BoxBV* target)
{
	// Create variables to perform appropriate tests
	BoxBV*		boxCollider = nullptr;
	SphereBV*	sphereCollider = nullptr;

	switch (m_collider->m_type)
	{
	// Cuboid types
	case PhysicsLib::LIGHT_BOX:
	case PhysicsLib::BOX:
	case PhysicsLib::DOOR:
	case PhysicsLib::COLOR_CUBE:
	case PhysicsLib::HOLED:
	case PhysicsLib::TELEPORTER:
	case PhysicsLib::FINAL_TOWER:
		boxCollider = dynamic_cast<BoxBV*>(m_collider);

		return CheckCollisions(target, boxCollider);

	// Sphere
	case PhysicsLib::SPHERE:
		sphereCollider = dynamic_cast<SphereBV*>(m_collider);

		return CheckCollisions(target, sphereCollider);

	// Invalid/irrelevant types
	default:
		return false;

	}
}

bool BVHierarchy::BVNode::CheckNodeType(SphereBV* target)
{
	// Create variables to perform appropriate tests
	BoxBV*		boxCollider = nullptr;
	SphereBV*	sphereCollider = nullptr;

	switch (m_collider->m_type)
	{
	// Cuboid types
	case PhysicsLib::LIGHT_BOX:
	case PhysicsLib::COLOR_CUBE:
	case PhysicsLib::DOOR:
	case PhysicsLib::BOX:
	case PhysicsLib::HOLED:
	case PhysicsLib::TELEPORTER:
	case PhysicsLib::FINAL_TOWER:
		boxCollider = dynamic_cast<BoxBV*>(m_collider);

		return CheckCollisions(target, boxCollider);

	// Sphere
	case PhysicsLib::SPHERE:
		sphereCollider = dynamic_cast<SphereBV*>(m_collider);

		return CheckCollisions(target, sphereCollider);

	// Invalid/irrelevant types
	default:
		return false;

	}
}

bool BVHierarchy::BVNode::CheckCollisions(SphereBV* target, BoxBV* collider)
{
	// Sphere/AABB test
	return SphereBV::CheckCollision(*target, *collider);
}

bool BVHierarchy::BVNode::CheckCollisions(SphereBV* target, SphereBV* collider)
{
	// Sphere/sphere test
	return SphereBV::CheckCollision(*target, *collider);
}


bool BVHierarchy::BVNode::CheckCollisions(BoxBV* target, BoxBV* collider)
{
	// AABB/AABB test
	return BoxBV::CheckCollision(*target, *collider);
}

bool BVHierarchy::BVNode::CheckCollisions(BoxBV* target, SphereBV* collider)
{
	// AABB/Sphere test
	return SphereBV::CheckCollision(*collider, *target);
}
