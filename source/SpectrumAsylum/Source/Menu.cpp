#define STB_IMAGE_IMPLEMENTATION

#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

#include "UI.hpp"
#include "Application.h"

#include "Game.h"
#include "LevelOne.h"

Game::Menu::Menu(Game& game)
{
	// Store window size
	m_screenSize = game.m_window.GetWindowSize();

	// Call title screen initializer to load background
	CreateStartMenu(game);
}

void Game::Menu::CreateStartMenu(Game& game)
{
	// Background
	m_background = 0;

	// Load texture for title screen background
	Texture background(std::string("TitleScreen.jpg"), &m_background);

	// Update boolean to unlock cursor
	game.m_window.m_lockCursor = false;

	// Update cursor mode to make it visible to the user
	glfwSetInputMode(game.m_window.m_windowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// Getter function for background id
unsigned int Game::Menu::GetBackgroundID(void) const
{
	return m_background;
}

// Reset background ID
void Game::Menu::ResetID(void)
{
	m_background = 0u;
}

void Game::Menu::UpdateStartMenu(Game& game)
{
	// Update current screen size
	m_screenSize = game.m_window.GetWindowSize();

	// Push ID to allow labels to be reused for multiple menus
	ImGui::PushID(TITLE_SCREEN);
	
	// Set UI window properties (position, size & transparency) relative to screen size
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(m_screenSize.m_x, m_screenSize.m_y));
	ImGui::SetNextWindowBgAlpha(0.0f);

	// Create window with required flags
	ImGui::Begin("Start Menu", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoScrollbar);
	
	// Get pointer for background window (opengl window)
	ImDrawList* bg = ImGui::GetBackgroundDrawList();
	bg->AddImage((void*) static_cast<intptr_t>(m_background), ImVec2(0, 0), ImVec2(m_screenSize.m_x, m_screenSize.m_y));

	// Set font to title font
	ImGui::PushFont(UIFonts::titleFont);
	
	// Set vertical spacing to be vertically centered
	ImGui::SetCursorPosY( m_screenSize.m_y * 0.5f);

	// Write title text
	ImGui::Text("Spectrum Asylum");
	
	// Stop using title font
	ImGui::PopFont();

	// Use sub-title font
	ImGui::PushFont(UIFonts::subTitleFont);

	// Multiply by coefficient to get screen horizontal spacing
	float coefficient = 0.02f;

	// Set vertical spacing between last & next component
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_screenSize.m_y * coefficient);
	
	// Draw play button
	if (ImGui::Button("Play"))
		PlayButton(true, game);

	// Set vertical spacing between last & next component
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_screenSize.m_y * coefficient);
	
	// Draw quit button
	if (ImGui::Button("Quit"))
		QuitButton(true, game.m_window.m_windowPtr);

	// Stop using sub title font
	ImGui::PopFont();

	// End of titlescreen menu
	ImGui::End();

	// Pop title screen ID
	ImGui::PopID();
}

void Game::Menu::UpdatePauseMenu(Game& game)
{
	// Update to current screen size
	m_screenSize = game.m_window.GetWindowSize();

	// Set window properties (position, scale, transparency) relative to screen size
	ImGui::SetNextWindowPos(ImVec2(m_screenSize.m_x / 5, 0));
	ImGui::SetNextWindowSize(ImVec2(m_screenSize.m_x / 4, m_screenSize.m_y));
	ImGui::SetNextWindowBgAlpha(0.85f);

	ImGui::PushID(PAUSE);

	// Create pause menu with required window flags
	ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoScrollbar);

	// Set title vertical spacing from top of the window
	ImGui::SetCursorPosY(m_screenSize.m_y / 5);

	// Assign title font as font for subsequent elements
	ImGui::PushFont(UIFonts::titleFont);

	/*
	* Set font size via screen width in order to scale relative to screen size
	* Multiply by 0.001 for font size relative to screen size
	*/
	UI::SetFontSize(UIFonts::titleFont, m_screenSize.m_x * 0.001f);

	// Draw text in UI window
	ImGui::Text("PAUSED");

	// Stop using current font (title font)
	ImGui::PopFont();

	// Assign current font to sub title font
	ImGui::PushFont(UIFonts::subTitleFont);

	// Set font size relative to screen size via screen width
	if (m_screenSize.m_x != 0)
		// Divide by 1200 for font scale relative to screen size
		UI::SetFontSize(UIFonts::subTitleFont, m_screenSize.m_x / 1200.0f);


	// Multiply by coefficient in order to get vertical spacing from top of screen
	float coefficient = 0.02f;

	// Set vertical spacing between last and next elements
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_screenSize.m_y * coefficient);

	// Create a button on UI menu with text 'Resume' & check if button or escape was pressed
	if (ImGui::Button("Resume") || Application::m_lockCursor)
		ResumeButton(true, game);

	// Set vertical spacing
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_screenSize.m_y * coefficient);

	// Create a button on UI menu with text 'Restart'
	if (ImGui::Button("Restart"))
		RestartButton(true, game);

	// Set vertical spacing between last and next elements
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_screenSize.m_y * coefficient);

	// Create a button on UI menu with text 'Quit'
	if (ImGui::Button("Quit"))
		QuitButton(true, game.m_window.m_windowPtr);

	// Stop using current font
	ImGui::PopFont();

	// End of window
	ImGui::End();

	ImGui::PopID();
}

void Game::Menu::UpdateEndMenu(Game& game)
{
	// Update to current screen size
	m_screenSize = game.m_window.GetWindowSize();
	
	// Assign button size relative to screen size
	ImVec2 buttonSize = {m_screenSize.m_x * 0.1f, m_screenSize.m_y * 0.1f};

	// Unlock cursor
	if (Application::m_lockCursor)
	{
		Application::m_lockCursor = false;
		glfwSetInputMode(game.m_window.m_windowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Set window configuration (position, size, transparency)
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(m_screenSize.m_x, m_screenSize.m_y));
	ImGui::SetNextWindowBgAlpha(0.9f);

	// Create unique ID to reuse labels
	ImGui::PushID(GAME_OVER);

	// Create pause menu with required window flags
	ImGui::Begin("End Menu", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoScrollbar);
	
	// Set font to title font
	ImGui::PushFont(UIFonts::titleFont);
	
	// Store title text & width of string
	const char title[] = "Level Complete";
	float titleWidth = ImGui::CalcTextSize(title).x;

	// Set position in UI window
	ImGui::SetCursorPos(ImVec2((m_screenSize.m_x - titleWidth) * 0.5f, m_screenSize.m_y * 0.33f));
	
	// Write text in UI
	ImGui::Text(title);

	// Stop using font
	ImGui::PopFont();

	// Button text
	const char restartText[] = "Restart";
	const char quitText[] = "Quit";

	// Set font to sub-title font
	ImGui::PushFont(UIFonts::subTitleFont);

	// Set position for next component
	ImGui::SetCursorPosX((0.5f * m_screenSize.m_x) - buttonSize.x);

	// Draw restart button
	if (ImGui::Button(restartText, buttonSize))
		RestartButton(true, game);
	
	// Set next UI component to same Y position
	ImGui::SameLine();

	// Set next button X position
	ImGui::SetCursorPosX(0.5f * (m_screenSize.m_x));

	// Draw quit button
	if (ImGui::Button(quitText, buttonSize))
		QuitButton(true, game.m_window.m_windowPtr);

	// Stop using sub-title font
	ImGui::PopFont();

	// End of end menu
	ImGui::End();
	ImGui::PopID();
}

void Game::Menu::DisplayMessage(std::string const& message, int id)
{
	// Store offset for window size 
	constexpr float offset = 10.0f;

	// Push ID to allow to reuse same labels
	ImGui::PushID(id);

	// Use sub-title font
	ImGui::PushFont(UIFonts::subTitleFont);

	// Calculate string size (store as ImVec2 in order to not recast later)
	ImVec2 messageSize = ImGui::CalcTextSize(message.c_str());

	// Set window property ( position, size, transparency)
	ImGui::SetNextWindowBgAlpha(0.75f);
	ImGui::SetNextWindowPos(ImVec2(0.5f * (m_screenSize.m_x - messageSize.x), 0.0f));
	ImGui::SetNextWindowSize(ImVec2{messageSize.x + offset, messageSize.y + offset});

	// Create UI window with flags
	ImGui::Begin("Display Message", nullptr, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoScrollbar);

	// Draw text in UI
	ImGui::Text(message.c_str());

	// End of UI window
	ImGui::End();

	// Stop using font & ID
	ImGui::PopFont();
	ImGui::PopID();
}

// Copy assignment operator
Game::Menu& Game::Menu::operator=(const Menu& rhs)
{
	m_screenSize = rhs.m_screenSize;
	m_background = rhs.m_background;

	return *this;
}

void Game::Menu::PlayButton(bool value, Game& game) const
{
	if (value)
	{
		std::cout << "Starting game...\n";

		// Set cursor position to last cursor position to prevent camera rotation once game resumes
		glfwSetCursorPos(game.m_window.m_windowPtr,
			static_cast<double>(Application::m_lastCursorPos.m_x),
			static_cast<double>(Application::m_lastCursorPos.m_y));

		// Lock cursor to stop cursor going out of the window
		Application::m_lockCursor = true;

		// Initialize level one
		InitLevelOne(game);

		// Update cursor mode (hide cursor)
		glfwSetInputMode(game.m_window.m_windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Update current state
		game.m_currentState = IN_GAME;
		
		// Delete titlescreen background
		glDeleteTextures(1, &m_background);
	}
}

void Game::Menu::ResumeButton(bool value, Game& game)
{
	if (value)
	{
		// Set cursor position to last cursor position to prevent camera rotation once game resumes
		glfwSetCursorPos(game.m_window.m_windowPtr,
			static_cast<double>(Application::m_lastCursorPos.m_x),
			static_cast<double>(Application::m_lastCursorPos.m_y));

		// Update boolean
		game.m_window.m_lockCursor = true;

		// Update cursor mode
		glfwSetInputMode(game.m_window.m_windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Update current state to game state
		game.m_currentState = IN_GAME;

		// Reset timer
		game.m_timer.GetDeltaTime();
		game.m_timer.SetNewFrame(true);
	}
}

void Game::Menu::RestartButton(bool value, Game& game)
{
	if (value)
	{
		std::printf("Restarting level...\n");

		// Set cursor position to last cursor position to prevent camera rotation once game resumes
		glfwSetCursorPos(game.m_window.m_windowPtr,
			static_cast<double>(Application::m_lastCursorPos.m_x),
			static_cast<double>(Application::m_lastCursorPos.m_y));

		// Update boolean
		game.m_window.m_lockCursor = true;

		// Update cursor mode
		glfwSetInputMode(game.m_window.m_windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Reset current level
		game.ResetLevel();

		// Re-initialize level one
		InitLevelOne(game);

		// Set current state to game state
		game.m_currentState = IN_GAME;
	}
}

void Game::Menu::QuitButton(bool value, GLFWwindow* windowPtr)
{
	// Close game
	glfwSetWindowShouldClose(windowPtr, value);
}

