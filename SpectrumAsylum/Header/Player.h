#pragma once

#include "PhysicsLib/CollisionDetection.h"
#include "PhysicsLib/ColliderHierarchy.hpp"

#include "ColorMacros.h"
#include "Color.hpp"
#include "Application.h"
#include "Camera.h"
#include "SceneNode.hpp"

class Player : public ISceneObject
{
public:
	// Constructors
				Player(void) = delete;
				Player(LibMath::Vector3 const& position, Camera* camera);
	
	// Destructor
				~Player(void);

	// Initialize Player
	static void InitPlayer(Graph<SceneNode>& gameObjects);

	// Player UI for debug information
	void		PlayerUI();

	// Setter functions
	void		SetCamera(Camera* camera);

	// Handle keyboard input for player movement
	void		ProcessKeyboardInput(KeyInput const& keyboardInput, float const& deltaTime);

	// Handle logic for player teleportation
	void		Teleport(Collider* const collider);

	// Handle logic for when player collides
	void		Collide(BVHierarchy& colliders);

	// Update player movement & rotation
	void		UpdatePlayer(Application& window, float deltaTime, BVHierarchy& colliders);

	// Cast a ray from the current player coordinates
	void		CastRay(Collider* collider, BoxBV*& outBox, float& outDistance);

	Camera*							m_camera;
	PhysicsLib::SphereCollider*		m_collider;
	Color							m_color = { WHITE, 1.f };
	LibMath::Vector3				m_position;
	LibMath::Vector3				m_velocity;

private:
	// Logic for player movement
	void		ApplyHorizontalVelocity(LibMath::Vector3 const& direction, float velocity, float const& deltaTime);
	void		ApplyVerticalVelocity(float const& velocity);
	void		UpdateMovement(float const& deltaTime);

	float		m_lastYPosition;
	float		m_playerHeight;

public:
	bool		m_isGrounded;
	bool		m_isTeleported = false;
	bool		m_insideTower = false;

};