#pragma once

#include "PhysicsLib/ColliderHierarchy.hpp"

#include "Graph.hpp"
#include "SceneNode.hpp"
#include "ResourceManager.h"

class Level
{
public:
	// Constructors
	Level(void) = default;
	Level(const Level&) = default;
	
	// Destructor
	~Level(void) = default;

	ResourceManager		m_assets;
	BVHierarchy			m_colliders;
	Graph<SceneNode>	m_scene;
};