#pragma once

#include <Vector>

#include "Player.h"
#include "Game.h"
#include"PhysicsLib/ColliderHierarchy.hpp"

// Init game sound engine
void InitSound(Game& game);

// Update game sound engine
void UpdateSound(Game& game);

// Load assets and build level
void InitLevelOne(Game& game);

// Place broad bounding volumes
void InitColliders(BVHierarchy& colliders);

// Place game objects into scene
void InitLevelOneHierarchies(Game& game);

// Place walls and floors in starting area (+x, +y, +z)
void InitStartingAreaWalls(Game& game);

// Place walls and floors in area 2 (-x, +y, +z)
void InitAreaTwoWalls(Game& game);

// Place walls and floors in area 3 (-x, +y, -z)
void InitAreaThreeWalls(Game& game);

// Place walls and floors in area 4 (+x, +y, -z)
void InitAreaFourWalls(Game& game);

// Place door underground walls and floor
void InitUnderground(Game& game);

// Place all level doors
void InitDoors(Game& game);

// Update game logic
void UpdateLevelOne(Game& game);

// Render level scene
void DrawScene(Game& game);


