#pragma once

#include "LibMath/Matrix/Matrix4.h"
#include "LibMath/Vector/Vector2.h"
#include "LibMath/Vector/Vector3.h"

#include "Application.h"
#include "SceneNode.hpp"
#include "Shader.h"
#include "Timer.hpp"
#include "Mesh.h"

#include "PhysicsLib/Frustum.h"

#define M_PI 3.14159f

class Camera : public ISceneObject
{
public:
	// Constructors
					 Camera(void) = delete;
					 Camera(const LibMath::Vector3& position, float fov, float moveIncrement, float pitchSpeed, float yawSpeed);

	// Destructor
					 ~Camera() = default;

	// Camera UI function
	void			 CameraUI(void);

	/// Getter functions
	LibMath::Vector3 GetFrontVector(void) const;
	LibMath::Vector3 GetRightVector(void) const;
	LibMath::Vector3 GetUpVector(void) const;
	float			 GetYaw(void) const;
	float			 GetPitch(void) const;

	// Setter functions
	void			 SetCameraPosition(LibMath::Vector3 const& position);
	void			 SetYaw(float const& radAngle);
	void			 SetPitch(float const& radAngle);

	// Update camera transform
	void			 UpdateCamera(Application& window, Shader* shader, Mesh& mesh);
	void			 UpdateCamera(Application& window);

	// Handle camera transformation
	void			 ProcessCameraRotation(float deltaX, float deltaY);
	void			 ProcessMouseMovement(const LibMath::Vector2& mousePos);

	// Get direction vector
	LibMath::Vector3 GetViewDirection(void);

	// Get 4x4 Matrices for MVP matrix
	LibMath::Matrix4 GetViewMatrix() const;
	LibMath::Matrix4 GetProjectionMatrix(const LibMath::Vector2& screenSize, float nearPlane, float farPlane);

	Frustum			 CameraFrustum(float aspect, float near, float far);


	LibMath::Matrix4  m_viewProjection;

	// Store last mouse position
	LibMath::Vector2 m_lastCursorPos = {0.0f, 0.0f};

	// Store camera position
	LibMath::Vector3 m_position;
	
	LibMath::Vector3 m_front;

private:
	void    UpdateVectors();

	// Camera relative axis vectors
	LibMath::Vector3 m_right;
	LibMath::Vector3 m_up;

	// Camera rotation & translation speed
	float m_movementIncrement;
	float m_pitchSpeed;
	float m_yawSpeed;

	// camera rotation
	float m_yaw;
	float m_pitch;

	// Camera field of view
	float m_fov;
};