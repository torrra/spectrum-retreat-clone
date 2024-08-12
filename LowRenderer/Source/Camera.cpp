#include <cmath>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <LibMath/Matrix/Matrix3.h>
#include <LibMath/Matrix/Matrix4.h>
#include <LibMath/Vector/Vector2.h>
#include <LibMath/Vector/Vector3.h>

#include "Camera.h"
#include "Timer.hpp"

#define ENABLE_CAMERA_MOVEMENT 0
#define ENABLE_CAMERA_ROTATE 1

Camera::Camera(const LibMath::Vector3& position, float fov, float moveIncrement, float pitchSpeed, float yawSpeed)
{
	// Camera vectors, used to determine camera orientation
	m_right = {1.0f, 0.0f, 0.0f};
	m_front = {-1.0f, 0.0f, 0.0f};
	m_up = {0.0f, 1.0f, 0.0f};

	// Camera position variables
	m_position = position;

	// Camera movement increment for each movement input
	m_movementIncrement = moveIncrement;

	// Camera rotation speed & angle
	m_pitchSpeed = pitchSpeed;
	m_yawSpeed = yawSpeed;
	m_pitch = 0.0f;
	m_yaw = 180.0f;

	// Camera field of view for perspective matrix
	m_fov = fov;

	m_type = CAMERA;
}

void Camera::CameraUI(void)
{
	int id = 0;
	ImGui::PushID(id);

	ImGui::BeginGroup();

	// Title
	ImGui::SeparatorText("Camera");

	// Local translation
	ImGui::Text("Local Position");
	ImGui::InputFloat("x position", &m_position.m_x);
	ImGui::InputFloat("y position", &m_position.m_y);
	ImGui::InputFloat("z position", &m_position.m_z);
	ImGui::Separator();

	ImGui::Text("Local Rotation");
	ImGui::InputFloat("Pitch", &m_pitch);
	ImGui::InputFloat("Yaw", &m_yaw);
	ImGui::Separator();

	ImGui::Text("Camera Velocity");
	ImGui::SliderFloat("Pitch Speed", &m_pitchSpeed, 0.0f, 1.0f);
	ImGui::SliderFloat("Yaw Speed", &m_yawSpeed, 0.0f, 1.0f);
	ImGui::SliderFloat("Movement Speed", &m_movementIncrement, 0.0f, 5.0f);
	ImGui::Separator();

	ImGui::Text("Camera FOV");
	ImGui::SliderFloat("FOV", &m_fov, 10.0f, 90.0f);
	ImGui::Separator();

	if (ImGui::Button("Reset Transformation"))
	{
		// Camera vectors, used to determine camera orientation
		m_right = {1.0f, 0.0f, 0.0f};
		m_front = {-1.0f, 0.0f, 0.0f};
		m_up = {0.0f, 1.0f, 0.0f};

		// Camera position variables
		m_position = {-1.0f, 1.0f, 3.5f};

		// Camera movement increment for each movement input
		m_movementIncrement = 1.f;

		// Camera rotation speed & angle
		m_pitchSpeed = 0.15f;
		m_yawSpeed = 0.1f;
		m_pitch = 0.0f;
		m_yaw = 270.0f;

		// Camera field of view for perspective matrix
		m_fov = 45.0f;
	}

	ImGui::EndGroup();

	// Allows us to reuse same IDs for multiple cameras
	ImGui::PopID();
}

LibMath::Vector3 Camera::GetFrontVector(void) const
{
	return m_front;
}

LibMath::Vector3 Camera::GetRightVector(void) const
{
	return m_right;
}

LibMath::Vector3 Camera::GetUpVector(void) const
{
	return m_up;
}

float Camera::GetYaw(void) const
{
	return m_yaw;
}

float Camera::GetPitch(void) const
{
	return m_pitch;
}

void Camera::SetYaw(float const& degAngle)
{
	m_yaw = degAngle;
}

void Camera::SetPitch(float const& degAngle)
{
	m_pitch = degAngle;
}

// Call function inside runtime loop in order to update the camera position & rotation
void Camera::UpdateCamera(Application& window, Shader* shader, Mesh& mesh)
{
	LibMath::Vector2 screenSize = window.GetWindowSize();

	// Check if cursor is in its locked state before updating camera movement or rotation
	if (window.m_lockCursor)
	{
		ProcessMouseMovement(window.GetMouseMotion());
	}

	// Update the data in the vertex shader
	shader->Use();

	LibMath::Matrix4 projection = GetProjectionMatrix(screenSize, 0.1f, 100.0f);
	LibMath::Matrix4 view = GetViewMatrix();

	// Calculate the MVP (model view projection) matrix
	LibMath::Matrix4 mvp = mesh.m_sceneNode->m_globalTransform * view * projection;

	//  Update the data for the view, projection & model 4x4 matrices inside the vertex shader

	LibMath::Matrix4 normalMat4 = mesh.m_sceneNode->m_globalTransform.GetInverse();
	normalMat4.Transpose(normalMat4);

	float normalMatArray[] =
	{
		normalMat4.m_matrix[0][0], normalMat4.m_matrix[0][1], normalMat4.m_matrix[0][2],
		normalMat4.m_matrix[1][0], normalMat4.m_matrix[1][1], normalMat4.m_matrix[1][2],
		normalMat4.m_matrix[2][0], normalMat4.m_matrix[2][1], normalMat4.m_matrix[2][2]
	};

	LibMath::Matrix3 normalMat3(normalMatArray);

	shader->SetUniform("model", mesh.m_sceneNode->m_globalTransform);
	shader->SetUniform("mvp", mvp);
	shader->SetUniform("normalMat", normalMat3);
}


void Camera::UpdateCamera(Application& window)
{
	// Check if cursor is in its locked state before updating camera movement or rotation
	if (window.m_lockCursor)
	{
		ProcessMouseMovement(window.GetMouseMotion());
	}

	LibMath::Matrix4 projection = GetProjectionMatrix(window.GetWindowSize(), 0.1f, 100.0f);
	LibMath::Matrix4 view = GetViewMatrix();

	// Calculate view projection matrix
   m_viewProjection =  view * projection;
}



void Camera::SetCameraPosition(LibMath::Vector3 const& position)
{
	m_position = position;
}

LibMath::Matrix4 Camera::GetViewMatrix(void) const
{
	// Calculate vectors for look at matrix
	const LibMath::Vector3 center = m_position + m_front;
	const LibMath::Vector3 forward = (center - m_position).normalizedCopy();
	const LibMath::Vector3 right = (forward.cross(m_up)).normalizedCopy();
	const LibMath::Vector3 up = right.cross(forward);

	/*
	*  Get view matrix values via an initializer list. Initialize the values in this
	*  specific order as all matrices are in column major.
	*/
	float matrix[4][4] =
	{
		{right.m_x, up.m_x, -forward.m_x, 0.0f},
		{right.m_y, up.m_y, -forward.m_y, 0.0f},
		{right.m_z, up.m_z, -forward.m_z, 0.0f},
		{-(right.dot(m_position)), -(up.dot(m_position)), forward.dot(m_position), 1.0f}
	};

	// Using initializer list create the view matrix
	return LibMath::Matrix4(matrix);
}

// Create a perspective 4x4 matrix
LibMath::Matrix4 Camera::GetProjectionMatrix(const LibMath::Vector2& screenSize, float nearPlane, float farPlane)
{
	// Calculate screen aspect ratio by diving the screen's width by the screen's height
	float aspect = screenSize.m_x / screenSize.m_y;

	// Create a 4x4 perspective matrix
	return LibMath::Matrix4::PerspectiveProjection(m_fov, aspect, nearPlane, farPlane);
}

Frustum Camera::CameraFrustum(float aspect, float near, float far)
{
	const LibMath::Vector3 center = m_position + m_front;
	const LibMath::Vector3 forward = (center - m_position).normalizedCopy();
	const LibMath::Vector3 right = (forward.cross(m_up)).normalizedCopy();
	const LibMath::Vector3 up = right.cross(forward);

	float halfFarHeight = far * tanf(m_fov * 0.5f);
	float halfFarWidth = halfFarHeight * aspect;

	const LibMath::Vector3 frontFar = far * m_front;

	Frustum frustum;

	frustum[NEAR] = { m_position + near * m_front, m_front };
	frustum[FAR] = { m_position + frontFar, -m_front };

	frustum[RIGHT] = { m_position, LibMath::Vector3(frontFar - right * halfFarWidth).cross(up) };
	frustum[LEFT] = { m_position, up.cross(LibMath::Vector3(frontFar + right * halfFarHeight)) };

	frustum[TOP] = { m_position, right.cross(LibMath::Vector3(frontFar - up * halfFarHeight)) };
	frustum[BOTTOM] = { m_position, LibMath::Vector3(frontFar + up * halfFarHeight).cross(right) };


	return frustum;
}

LibMath::Vector3 Camera::GetViewDirection(void)
{
	return m_front;
}

// Calculate the camera's current rotation
void Camera::ProcessCameraRotation(float deltaX, float deltaY)
{
	// Scale the rotation amount
	deltaX *= m_yawSpeed;
	deltaY *= m_pitchSpeed;

	// Convert the change in mouse position to the rotation angle
	m_yaw += deltaX;
	m_pitch += deltaY;

	// Clamp pitch rotation to stop camera going upside down
	if ((m_pitch - deltaY) < -80.0f)
		m_pitch = -80.0f;
	if ((m_pitch - deltaY) > 80.0f)
		m_pitch = 80.0f;

	// Clamp yaw rotation from -360 to +360 degrees
	m_yaw = ((int) m_yaw % 360) + (m_yaw - (int) m_yaw);

	// Update front, right & up vector due to the change in the camera's orientation
	UpdateVectors();
}

// Calculate delta mouse position
void Camera::ProcessMouseMovement(const LibMath::Vector2& mousePos)
{
	// Calculate the difference in mouse position
	float deltaX = mousePos.m_x - m_lastCursorPos.m_x;
	float deltaY = m_lastCursorPos.m_y - mousePos.m_y;

	m_lastCursorPos.m_x = mousePos.m_x;
	m_lastCursorPos.m_y = mousePos.m_y;

	// Via the delta mouse position, update the camera's rotation
	ProcessCameraRotation(deltaX, deltaY);
}

// Recalculate the camera's front, right & up vectors
void Camera::UpdateVectors()
{
	// Update camera vectors
	LibMath::Vector3 front = {0.0f, 0.0f, 0.0f};

	// Update front vector relative to the camera's current transformation
	front.m_x = cosf(m_yaw * (M_PI / 180.0f)) * cosf(m_pitch * (M_PI / 180.0f));
	front.m_y = sinf(m_pitch * (M_PI / 180.0f));
	front.m_z = sinf(m_yaw * (M_PI / 180.0f)) * cosf(m_pitch * (M_PI / 180.0f));

	// Update & normalize all vectors via the updated front vector
	m_front = front.normalizedCopy();
	m_right = (m_front.cross({0.0f, 1.0f, 0.0f})).normalizedCopy();
	m_up = (m_right.cross(m_front)).normalizedCopy();
}