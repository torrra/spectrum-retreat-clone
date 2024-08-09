#pragma once

#include "Graph.hpp"
#include "Node.h"
#include "CollisionDetection.h"

#include "Frustum.h"

// Collider graph container for broad phase sweeping and easier updates
class BVHierarchy
{
public:

	// Default constructor and destructor
	BVHierarchy(void) = default;
	~BVHierarchy(void) = default;

	// Add a collider of any typo into graph with a parent
	template <typename Tobj, typename... TArgs>
	Tobj* AddCollider(const std::string parentKey, const std::string key, const TArgs&... args)
	{
		return m_hierarchy.AddChild<Tobj>(parentKey, key, args...);
	}


	// Add a collider of any type into graph without a parent
	template <typename Tobj, typename... TArgs>
	Tobj* AddCollider(const std::string key, const TArgs&... args)
	{
		return m_hierarchy.AddChild<Tobj>(key, args...);
	}


	// Delete a collider from graph
	void DeleteCollider(const std::string key)
	{
		m_hierarchy.DeleteNode(key);
	}

	// Save mesh scene node associated with collider
	template<class NodeT>
	void LinkToNode(Graph<NodeT>& graph, const std::string colliderKey, const std::string sceneNodeKey);

	// Update entire hierarchy
	void Update()
	{
		m_hierarchy.UpdateGraph();
	}

	// Update entire hierarchy with frustum culling
	void Update(const Frustum& frustum)
	{
		// Iterate through root children and recursively update graph
		for (BVNode* node : m_hierarchy.m_worldRoot->m_children)
		{
			node->Update(frustum);
		}
	}

	// Collider node class
	class BVNode
	{
	public:

		// Constructors
		BVNode(void) = default;
		BVNode(BVNode* parent, Collider* collider);

		// Destructor
		~BVNode(void);

		// Update collider with frustum culling
		void Update(const Frustum& frustum);

		// Update collider
		void Update();

		// Update AABB
		void UpdateBox();

		// Update AABB with frustum culling
		void UpdateBox(const Frustum& frustum);

		// Update bounding sphere
		void UpdateSphere();

		// Test collision with any collider type
		bool TestCollision(Collider* target);

		// Sweep and prune colliders
		void PruneColliders(Collider* target, std::vector<BVHierarchy::BVNode*>& prunedList, BVHierarchy::BVNode* collider);


		std::vector<BVNode*>	m_children;
		BVNode*					m_parent = nullptr;
		Node*					m_sceneNode = nullptr;
		Collider*				m_collider = nullptr;


	private:

		// Check this node type depending on target type
		bool CheckNodeType(BoxBV* target);
		bool CheckNodeType(SphereBV* target);

		// Check collisions against a spherical target
		bool CheckCollisions(SphereBV* target, BoxBV* collider);
		bool CheckCollisions(SphereBV* target, SphereBV* collider);

		// Check collisions against a cuboid target
		bool CheckCollisions(BoxBV* target, BoxBV* collider);
		bool CheckCollisions(BoxBV* target, SphereBV* collider);
	};


	Graph<BVNode>			m_hierarchy;

};

// Store scene node address in BVNode
template<class NodeT>
void BVHierarchy::LinkToNode(Graph<NodeT>& graph, const std::string colliderKey, const std::string sceneNodeKey)
{
	BVNode* node = m_hierarchy.GetNode(colliderKey);

	node->m_sceneNode = graph.GetNode(sceneNodeKey);


	node->Update();
}

// Get cast collider from collider hierarchy
template<class Tobj>
Tobj* GetObject(Graph<BVHierarchy::BVNode>& graph, std::string key)
{
	Collider* obj = graph.GetNode(key)->m_collider;

	return dynamic_cast<Tobj*>(obj);
}