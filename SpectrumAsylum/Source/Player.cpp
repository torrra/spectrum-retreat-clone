#include "imgui/imgui.h"

#include "PhysicsLib/CollisionDetection.h"
#include "PhysicsLib/RayCast.h"

#include "DoorLogic.h"
#include "Game.h"
#include "LightVolume.h"
#include "Player.h"

Player::Player(LibMath::Vector3 const& position, Camera* camera)
{
	// Set ISceneObject object type to player
	m_type = PLAYER;

	// Set initial position & velocity
	m_position = position;
	m_velocity = LibMath::Vector3::zero();

	// Set player height & last y position
	m_playerHeight = 1.0f;
	m_lastYPosition = 0.0f;

	// Set camera
	m_camera = camera;

	// Add a sphere collider for the player
	m_collider = new PhysicsLib::SphereCollider(0.75f, m_position);

	// Set false therefore player is falling on game start
	m_isGrounded = false;

	// Set initial phone color
	Door::m_playerColor = &m_color;
}

Player::~Player(void)
{
	// Check if player sphere collider is not null & if not null delete it
	if(m_collider)
		delete m_collider;
}

void Player::InitPlayer(Graph<SceneNode>& gameObjects)
{
	// Initialize player

	// Add camera to scene graph
	Camera* camera = gameObjects.AddChild<Camera>("camera",
		LibMath::Vector3{45.f, 1.476f, 45.f},
		45.0f, 1.f, 0.15f, 0.1f);

	// Add player to scene graph
	gameObjects.AddChild<Player>("player", camera->m_position, camera);
}

void Player::PlayerUI()
{
	// Debug UI to display information about the player

	// Set window size (in pixels)
	ImGui::SetNextWindowSize(ImVec2(200, 300));

	// Create window with given flags
	ImGui::Begin("Player", nullptr, ImGuiWindowFlags_NoResize);

	// Local translation
	ImGui::SeparatorText("Player Position");
	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("x", &m_position.m_x);

	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("y", &m_position.m_y);

	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("z", &m_position.m_z);

	// Local rotation
	ImGui::SeparatorText("Camera Direction");
	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("x direction", &m_camera->m_front.m_x);

	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("y direction", &m_camera->m_front.m_y);

	ImGui::SetNextItemWidth(100.f);
	ImGui::InputFloat("z direction", &m_camera->m_front.m_z);

	// End of player UI window
	ImGui::End();
}

void Player::SetCamera(Camera* camera)
{
	// Assign camera
	m_camera = camera;
}

void Player::ProcessKeyboardInput(KeyInput const& keyboardInput, float const& deltaTime)
{
	// Assign horizontal & vertical player speed
	constexpr float verticalVelocity = 5.0f;
	constexpr float horizontalVelocity = 10.0f;

	// Get front and right camera vectors
	LibMath::Vector3 frontDir = m_camera->GetFrontVector();
	LibMath::Vector3 rightDir = m_camera->GetRightVector();

	// Update the player's position in relation to the keys pressed
	if (keyboardInput.forward)
		ApplyHorizontalVelocity(frontDir, horizontalVelocity, deltaTime);
	else if (keyboardInput.back)
		ApplyHorizontalVelocity(-frontDir, horizontalVelocity, deltaTime);

	if (keyboardInput.right)
		ApplyHorizontalVelocity(rightDir, horizontalVelocity, deltaTime);
	else if (keyboardInput.left)
		ApplyHorizontalVelocity(-rightDir, horizontalVelocity, deltaTime);

	if (keyboardInput.jump)
		ApplyVerticalVelocity(verticalVelocity);
}


void Player::Teleport(Collider* const collider)
{
	// Cast ICollider to Teleporter colldier
	PhysicsLib::Teleporter*		teleporter = dynamic_cast<PhysicsLib::Teleporter*>(collider);

	// Stop if dynamic cast failed
	if (!teleporter)
		return;

	// Only teleport player once
	if (!m_isTeleported)
	{
		// Set new player position
		m_position = teleporter->m_otherSide->m_position;

		// Set player rotation for after the teleportation
		m_camera->SetYaw(-180.0f);
		m_camera->SetPitch(0.0f);

		// Updtate variable saying player has been successfully teleported
		m_isTeleported = true;
	}
}

void Player::Collide(BVHierarchy& colliders)
{
	std::vector<BVHierarchy::BVNode*>	prunedList;

	BVHierarchy::BVNode*				world = colliders.m_hierarchy.GetNode("world");

	world->PruneColliders(m_collider, prunedList, world);

	BoxBV* colorToSwap = nullptr;
	float shortestDistance = 0.0f;

	// Iterate through all colliders
	for (BVHierarchy::BVNode* collider : prunedList)
	{
		CastRay(collider->m_collider, colorToSwap, shortestDistance);

		// Sphere collider
		if (collider->TestCollision(m_collider))
		{

			if (collider->m_collider->m_type == PhysicsLib::LIGHT_BOX)
			{
				LightBox* lightBox = dynamic_cast<LightBox*>(collider->m_collider);

				lightBox->EnableLights();
			}

			// Check for collision with teleporter
			else if (collider->m_collider->m_type == PhysicsLib::TELEPORTER)
				Teleport(collider->m_collider);

			// Check for collision in area 'Final Tower'
			else if (collider->m_collider->m_type == PhysicsLib::FINAL_TOWER)
			{
				m_insideTower = true;

				if (Application::m_shiftPressed)
				{
					// Set player position
					m_position = { 10.42f, 7.9f, 12.846f };

					// Set player rotation
					m_camera->SetYaw(90.f);
					m_camera->SetPitch(0.f);
				}
			}
			else
			{
				// Collision has occured therefore check each axis seperatly for collision in order to apply slide against surface

				// X axis
				m_collider->m_position = { m_position.m_x, m_camera->m_position.m_y - m_playerHeight, m_camera->m_position.m_z };
				m_position.m_x = (collider->TestCollision(m_collider)) ? m_camera->m_position.m_x : m_position.m_x;

				// Y axis
				m_collider->m_position = { m_camera->m_position.m_x, m_position.m_y, m_camera->m_position.m_z };
				m_position.m_y = (collider->TestCollision(m_collider)) ? m_camera->m_position.m_y - m_playerHeight : m_position.m_y;

				// Z axis
				m_collider->m_position = { m_camera->m_position.m_x, m_camera->m_position.m_y - m_playerHeight, m_position.m_z };
				m_position.m_z = (collider->TestCollision(m_collider)) ? m_camera->m_position.m_z : m_position.m_z;

				// Update player collider location
				m_collider->m_position = { m_position.m_x, m_position.m_y, m_position.m_z };

				if (collider->m_collider->m_type == PhysicsLib::DOOR)
					Game::m_doorHit = true;
			}
		}
		else if (collider->m_collider->m_type == PhysicsLib::LIGHT_BOX)
		{
			LightBox* lightBox = dynamic_cast<LightBox*>(collider->m_collider);

			lightBox->DisableLights();
		}
		else if (collider->m_collider->m_type == PhysicsLib::FINAL_TOWER)
		{
			m_insideTower = false;
		}
	}

	// Disable raycast
	if (Application::m_leftClick)
		Application::m_leftClick = false;

	Application::m_shiftPressed = false;

	// Check color can be swap with given cube
	if (colorToSwap && colorToSwap->m_type == PhysicsLib::COLOR_CUBE)
	{
		ColBoxBV* coloredBox = dynamic_cast<ColBoxBV*>(colorToSwap);

		coloredBox->m_color->Swap(m_color);

		Door::m_playerColor = &m_color;
		Application::m_lastRayHit = glfwGetTime();
	}
}

void Player::UpdatePlayer(Application& window, float deltaTime, BVHierarchy& colliders)
{
	// Get keyboard input
	KeyInput keyInput = window.GetKeyBoardInput();

	// Call keyboard inputs
	ProcessKeyboardInput(keyInput, deltaTime);

	// Update player transform
	UpdateMovement(deltaTime);

	// Update player collider position
	m_collider->m_position = m_position;


	// Check for collision
	Collide(colliders);

	// Update camera position
	m_camera->m_position = {m_position.m_x, m_position.m_y + m_playerHeight, m_position.m_z};
}


void Player::CastRay(Collider* collider, BoxBV*& outBox, float& outDistance)
{
	// Do not test intersections if left mouse button was not clicked
	if (!Application::m_leftClick)
		return;

	// Create ray going from player position going along camera direction
	Ray			colorRay({ m_position.m_x, m_position.m_y + m_playerHeight, m_position.m_z }, m_camera->GetFrontVector());

	// Cast ICollider object into BoxCollider
	BoxBV*		box = dynamic_cast<BoxBV*>(collider);

	// Intersection distance to be written into
	float	distance;

	// Do not perform any further test if no ray intersection occured
	if (!box || !colorRay.Intersect(*box, distance))
		return;

	// Apply special conditions to colliders with a hole
	if (box->m_type == PhysicsLib::HOLED)
	{
		// Cast BoxCollider to Holedcollider (box with a hole)
		PhysicsLib::HoledCollider*		holed = dynamic_cast<PhysicsLib::HoledCollider*>(box);

		// Player-hole distance variable to be written into
		float dHole;

		// Only record intersection if ray does not pass through hole
		if (!colorRay.Intersect(*holed->m_hole, dHole))
		{
			// Update closest AABB and shortest distance variable
			if (!outBox || distance < outDistance)
			{
				outBox = box;
				outDistance = distance;
			}
		}
	}

	// If an intersection occurred with a plain AABB (no hole, light, volume or teleporter)
	else if (collider->m_type == PhysicsLib::BOX ||
			 collider->m_type == PhysicsLib::COLOR_CUBE ||
			 collider->m_type == PhysicsLib::DOOR)
	{
		// Update closest AABB and shortest distance variables
		if (!outBox || distance < outDistance)
		{
			outBox = box;
			outDistance = distance;
		}
	}
}

void Player::ApplyHorizontalVelocity(LibMath::Vector3 const& direction, float velocity, float const& deltaTime)
{

	// Create a horizontal direction vector so not to affect the current player height
	LibMath::Vector3 dirVector = {direction.m_x, 0.0f, direction.m_z};

	// Apply scale to velocity depending if player is on the ground or in the air
	float airVelocity = m_isGrounded ? 1.0f : 0.6f;

	// Update position in the given direction with the given velocity
	m_position += dirVector * (airVelocity * velocity) * deltaTime;
}

void Player::ApplyVerticalVelocity(float const& velocity)
{
	// Check player is on the ground
	if (!m_isGrounded)
		return;

	// Apply up velocity
	m_velocity.m_y = velocity;
}

void Player::UpdateMovement(float const& deltaTime)
{
	// Store value for gravity
	constexpr float accel = -9.81f;

	// Check if the player is currently on the ground
	m_isGrounded = (m_position.m_y == m_lastYPosition) ? false : true;

	 // Check if gravity should be applied
	if (!m_isGrounded)
	{
		// Calculate vertical velocity due to gravity
		m_velocity.m_y += accel * deltaTime;

		// Clamp max velocity
		m_velocity.m_y = (m_velocity.m_y >= accel) ? m_velocity.m_y : accel;

		// Restart level if player falls off
		if (m_position.m_y <= -20.0f)
			Game::m_gameOver = true;
	}

	// Set current player height
	m_position.m_y += m_velocity.m_y * deltaTime;

	// Update the value for the last player height
	m_lastYPosition = m_position.m_y;
}
