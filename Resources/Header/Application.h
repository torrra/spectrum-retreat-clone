#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "LibMath/Vector/Vector2.h"
#include "LibMath/Vector/Vector4.h"

struct KeyInput
{
	bool forward = false;
	bool back = false;
	bool left = false;
	bool right = false;
	bool jump = false;
};

enum RENDER_MODE
{
	FILL,
	LINE
};

// Framebuffer resize callback
void ResizeWindow(GLFWwindow* window, int width, int height);

// Mouse click process callback
void ProcessMousePress(GLFWwindow* window, int button, int action, int mods);
void ProcessKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);

// GLFW context (window)
class Application
{
public:
	// Create a default window
	Application(void);

	// Create a custom window
	Application(int width, int height, const char* windowTitle);

	// Destroy window
	~Application(void);

	// Swap buffers and poll events
	void UpdateWindow(void);

	// Get keyboard input for player movement
	KeyInput GetKeyBoardInput(void);

	LibMath::Vector2 GetMouseMotion(void) const;

	// Get Window width and height
	LibMath::Vector2 GetWindowSize(void) const;


	float GetAspectRatio(void) const;


	void SetWindowColor(LibMath::Vector4 const& bgColor, RENDER_MODE const& renderMode);

	// Returns true if ESC is pressed or close button is clicked on
	bool WindowShouldClose(void);

	// Check if window should be kept open
	operator bool();

	// Pointer to GLFW context
	GLFWwindow*					m_windowPtr;

	static LibMath::Vector2		m_lastCursorPos;
	static double				m_lastRayHit;

	// Hide and keep cursor within window
	static bool					m_lockCursor;
	static bool					m_leftClick;
	static bool					m_pauseButton;
	static bool					m_shiftPressed;

private:

	// Init GLFW context and load GLAD
	void InitWindow(void);


	const char*		m_windowTitle;
	int				m_screenWidth;
	int				m_screenHeight;


};