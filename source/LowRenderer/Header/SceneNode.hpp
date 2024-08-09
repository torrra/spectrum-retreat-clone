#pragma once

#include "Node.h"
#include "Graph.hpp"

enum OBJECT_TYPE
{
	MESH,
	LIGHT,
	CAMERA,
	PLAYER,
	COLOR_BLOCK,
	DOOR
};

// Forward class declaration
class SceneNode;

// Interface for scene objects
class ISceneObject
{
public:
	// Constructor
	ISceneObject() = default;

	// Virtual destructor
	virtual ~ISceneObject() = default;

	virtual void LinkToNode(SceneNode* node) {}

	OBJECT_TYPE m_type = MESH;
};

class SceneNode : public Node
{
public:
	// Constructor
	SceneNode(SceneNode* parent, ISceneObject* object)
		: m_object(object)
	{
		m_parent = parent;

		object->LinkToNode(this);
	}

	SceneNode(SceneNode* parent)
	{
		// Set parent
		m_parent = parent;
	}


	// Destructor
	~SceneNode(void)
	{
		// Check object is not a null pointer
		if (m_object)
			delete m_object;

		// Delete all child nodes
		for (Node* child : m_children)
		{
			if (child)
			{
				SceneNode* castChild = dynamic_cast<SceneNode*>(child);

				castChild->~SceneNode();
				delete child;
			}
		}

		m_children.~vector();
	}

	ISceneObject* m_object = nullptr;
};


template<class Tobj>
Tobj* GetObject(Graph<SceneNode>& graph, std::string key)
{
	// Get object node
	ISceneObject* obj = graph.GetNode(key)->m_object;

	return dynamic_cast<Tobj*>(obj);
}