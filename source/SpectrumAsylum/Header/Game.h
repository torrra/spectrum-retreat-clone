#pragma once

// IrrKlang
#include <irrKlang/irrKlang.h>

// Engine resources & low renderer
#include "Application.h"
#include "Timer.hpp"
#include "Mesh.h"

// Game
#include "DoorLogic.h"
#include "ColorBlock.h"
#include "Level.h"
#include "Game.h"


// Macros
#include "ColorMacros.h"

// Enumerator to switch with the different states of game
enum GameState
{
	TITLE_SCREEN,
	IN_GAME,
	PAUSE,
	GAME_OVER

}; // !Enum GameState

class Game
{
public:

	// Constructor
	Game(void);

	// Destructor
	~Game(void);

	// Create functions
	// Create wall mesh with a key, a model, a position, a scale, a material, and an area for the collider by default to world
	Mesh*	CreateWall(const std::string& key, Model* model, const char* modelString, const LibMath::Vector3& pos, const LibMath::Vector3& scale, Material* mat, const char* areaKey = "world");

	// Create wall with a hole mesh with a key, a model, a position, a scale, a material, and an area for the collider by default to world
	Mesh*	CreateWallWithHole(const std::string& key, Model* model, const char* modelString, const LibMath::Vector3& pos, const LibMath::Vector3& scale, Material* mat, const char* areaKey = "world");

	// Create floor mesh with a key, a model, a position, a scale, a material, and an area for the collider by default to world
	Mesh*	CreateFloor(const std::string& key, Model* model, const LibMath::Vector3& pos, const LibMath::Vector3& scale, Material* mat, const char* areaKey = "world");

	// Create all light for the level with a key, a position, a material for the mesh, material for the edge*/
	PointLight*	CreateLevelLight(const std::string& key, const LibMath::Vector3& pos, Material* meshMat, Material* edgeMat);

	// Create color block with a key, a position, a scale, a color, a model and an area for the collider by default to world
	ColorBlock*	CreateColorBlock(const std::string& key, const LibMath::Vector3& pos, const Color& color, Model* model, const char* areaKey = "world");

	// Create a mesh of a cube holder with a key, a model, a position and an area for the collider by default to world
	Mesh*	CreateCubeHolder(const std::string& key, Model* model, const LibMath::Vector3& pos, const char* areaKey = "world");

	// Create a door with a key, a position, a scale, a color, and an area for the collider by default to world
	Door*	CreateDoor(const std::string& key, const LibMath::Vector3& pos, const LibMath::Vector3& scale, const Color& color, Model* model, const char* areaKey = "world");

	// Create a teleporter with a key, a position, an other position, and an area for the collider by default world
	PhysicsLib::Teleporter*	CreateTeleporters(const std::string& key, const LibMath::Vector3& pos, const LibMath::Vector3& otherPos, const char* areaKey = "world");

	// Reset level
	void ResetLevel(void);

	// Update switch between the different game states
	void Update();

	// Nested class Menu
	class Menu
	{
	public:
		// Constructor
		Menu(void) = default;
		Menu(Game& game);
		Menu(const Menu&) = default;

		// Destructor
		~Menu(void) = default;

		// Getter
		unsigned int GetBackgroundID(void) const;

		// Reset
		void ResetID(void);

		// Update
		void UpdateStartMenu(Game& game);
		void UpdatePauseMenu(Game& game);
		void UpdateEndMenu(Game& game);
		void DisplayMessage(std::string const& message, int id);

		// Comparator
		Menu& operator=(const Menu& rhs);

	private:

		// Create
		void CreateStartMenu(Game& game);

		// Button
		void PlayButton(bool value, Game& game) const;
		void ResumeButton(bool value, Game& game);
		void RestartButton(bool value, Game& game);
		void QuitButton(bool value, GLFWwindow* windowPtr);

		// Variables
		LibMath::Vector2	m_screenSize;
		GLuint				m_background	= 0;

	}; // !Class Menu

	// Variables

	std::ofstream			m_sceneFile;
	irrklang::ISoundEngine*	m_soundEngine	= nullptr;
	Level					m_currentLevel;
	Application				m_window;
	Timer					m_timer;
	Menu					m_menu;
	GameState				m_currentState	= TITLE_SCREEN;
	static bool				m_doorHit;
	static bool				m_gameOver;

}; // !Class Game