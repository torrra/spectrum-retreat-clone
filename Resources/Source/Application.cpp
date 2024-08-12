#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"


#include "LibMath/Vector/Vector3.h"

#include "Application.h"
#include "Timer.hpp"
#include "UI.hpp"


bool				Application::m_lockCursor = true;
bool				Application::m_shiftPressed = false;
LibMath::Vector2	Application::m_lastCursorPos = { 0.0f, 0.0f };
double				Application::m_lastRayHit;

bool Application::m_leftClick = false;
bool Application::m_pauseButton = false;

Application::Application(void)
{
	// Assign default values

	m_windowPtr = nullptr;
	m_windowTitle = "Spectrum Asylum";
	m_screenWidth = 1280;
	m_screenHeight = 720;

	// Init GLFW context
	InitWindow();
}

Application::Application(int width, int height, const char* windowTitle)
{
	// Assign passed values

	m_windowPtr = nullptr;
	m_windowTitle = windowTitle;
	m_screenWidth = width;
	m_screenHeight = height;

	// Init GLFW context
	InitWindow();
}

Application::~Application(void)
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	// Destroy window
	glfwDestroyWindow(m_windowPtr);

	// Terminale GLFW context
	glfwTerminate();
}

void Application::InitWindow(void)
{
	// Initialize GLFW
	glfwInit();

	// GLFW version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	m_windowPtr = glfwCreateWindow(m_screenWidth, m_screenHeight, m_windowTitle, NULL, NULL);

	// Check window is not nullptr
	if (m_windowPtr == nullptr)
	{
		std::cerr << "Window is nullptr";
		glfwTerminate();
		std::exit(-1);
	}

	glfwMakeContextCurrent(m_windowPtr);

	// Load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	// Register callbacks
	glfwSetFramebufferSizeCallback(m_windowPtr, ResizeWindow);
	glfwSetMouseButtonCallback(m_windowPtr, ProcessMousePress);
	glfwSetKeyCallback(m_windowPtr, ProcessKeyPress);

	// Disable cursor
	glfwSetInputMode(m_windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	UI::SetFont(io, 20);
	UI::DarkTheme();
	ImGui_ImplGlfw_InitForOpenGL(m_windowPtr, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::UpdateWindow(void)
{
	// Swap front and back buffers
	glfwSwapBuffers(m_windowPtr);

	// Update size
	glfwGetWindowSize(m_windowPtr, &m_screenWidth, &m_screenHeight);

	// Check for events
	glfwPollEvents();

}

bool Application::WindowShouldClose(void)
{
	// Check if esc has been pressed
	int state = glfwGetKey(m_windowPtr, GLFW_KEY_P);

	// Tell user that window should be closed if esc was pressed or if close button was clicked on
	if (state == GLFW_PRESS || glfwWindowShouldClose(m_windowPtr))
		return true;

	return false;

}

KeyInput Application::GetKeyBoardInput(void)
{
	KeyInput keyInput;

	// Stop camera rotation updates when cursor is in its unlocked state
	if (!Application::m_lockCursor)
		return keyInput;

	// Check if the player is moving
	keyInput.forward = (glfwGetKey(m_windowPtr, GLFW_KEY_W) == GLFW_PRESS);
	keyInput.back = (glfwGetKey(m_windowPtr, GLFW_KEY_S) == GLFW_PRESS);
	keyInput.left = (glfwGetKey(m_windowPtr, GLFW_KEY_A) == GLFW_PRESS);
	keyInput.right = (glfwGetKey(m_windowPtr, GLFW_KEY_D) == GLFW_PRESS);
	keyInput.jump = (glfwGetKey(m_windowPtr, GLFW_KEY_SPACE) == GLFW_PRESS);

	if (glfwGetKey(m_windowPtr, GLFW_KEY_F) == GLFW_PRESS)
		glfwMaximizeWindow(m_windowPtr);

	if (glfwGetKey(m_windowPtr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		m_shiftPressed = true;

	return keyInput;
}

LibMath::Vector2 Application::GetMouseMotion(void) const
{
	// Cursor x and y position
	double	pitch = 0.00;
	double	yaw = 0.00;

	// Retrieve position
	glfwGetCursorPos(m_windowPtr, &yaw, &pitch);

	// Return position
	return LibMath::Vector2(static_cast<float>(yaw), static_cast<float>(pitch));
}

LibMath::Vector2 Application::GetWindowSize() const
{
	// Return window width and height
	return LibMath::Vector2{ static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight) };
}

float Application::GetAspectRatio(void) const
{
	return m_screenWidth / m_screenHeight;
}


void Application::SetWindowColor(LibMath::Vector4 const& bgColor, RENDER_MODE const& renderMode)
{
	auto mode = (renderMode == RENDER_MODE::FILL) ? GL_FILL : GL_LINE;

	// Set screen color
	glClearColor(bgColor.m_x, bgColor.m_y, bgColor.m_z, bgColor.m_w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, mode);
}

Application::operator bool()
{
	// Tell user window should close
	if (glfwWindowShouldClose(m_windowPtr))
		return false;

	return true;
}

void ResizeWindow(GLFWwindow* window, int width, int height)
{
	// Update viewport
	glViewport(0, 0, width, height);
}

void ProcessMousePress(GLFWwindow* window, int button, int action, int mods)
{
	// 0 = left click
	if (button == 0 && action == GLFW_RELEASE)
	{
		Application::m_leftClick = true;
	}
	// 1 = right click
	else if (button == 1 && action == GLFW_RELEASE)
	{
	}
	else
		Application::m_leftClick = false;
}

void ProcessKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
	{
		// Change cursor state
		Application::m_lockCursor = !Application::m_lockCursor;

		Application::m_pauseButton = !Application::m_pauseButton;

		// Get cursor position
		if (!Application::m_lockCursor)
		{
			double	xPos = 0.00;
			double	yPos = 0.00;

			// Get cursor position
			glfwGetCursorPos(window, &xPos, &yPos);

			int width, height;

			glfwGetWindowSize(window, &width, &height);

			// Record position for later
			Application::m_lastCursorPos = {static_cast<float>(xPos), static_cast<float>(yPos)};
		}
		else
		{
			// Assign previously recorded position
			glfwSetCursorPos(window, static_cast<double>(Application::m_lastCursorPos.m_x), static_cast<double>(Application::m_lastCursorPos.m_y));
		}

		// Update boolean
		int setMode = Application::m_lockCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;

		// Update cursor mode
		glfwSetInputMode(window, GLFW_CURSOR, setMode);
	}
}
