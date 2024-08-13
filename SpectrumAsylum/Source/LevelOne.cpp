#include "imgui/imgui.h"

#include "LibMath/Angle/Degree.h"
#include "LibMath/Trigonometry.h"

#include "PhysicsLib/ColliderHierarchy.hpp"

#include "Application.h"
#include "Model.h"
#include "Mesh.h"
#include "Camera.h"

#include "ColorMacros.h"
#include "DoorLogic.h"
#include "Player.h"
#include "LevelOne.h"
#include "Scene.h"
#include "LevelOneLights.h"

void InitSound(Game& game)
{
	// Create irrklang sound device
	game.m_soundEngine = irrklang::createIrrKlangDevice();

	// Exit program on failure
	if (!game.m_soundEngine)
	{
		std::printf("Could not startup engine\n");
		std::exit(-1); // error starting up the engine
	}
}

void UpdateSound(Game& game)
{
	// Variables for SFX cooldown
	double				currentTime = glfwGetTime();
	static double		lastTime = 0.0;

	// Play 'pew  pew' sound upon ray cast
	if (Application::m_leftClick && game.m_currentState == IN_GAME)
	{
		game.m_soundEngine->setSoundVolume(static_cast<irrklang::ik_f32>(0.35));
		game.m_soundEngine->play2D("assets/Sound/raycast.wav");
	}

	// Play blocked door sound every second when hitting a closed door
	if (Game::m_doorHit && game.m_currentState == IN_GAME)
	{
		// Only play if a second has passed
		if (currentTime - lastTime >= 1.0)
		{
			game.m_soundEngine->setSoundVolume(static_cast<irrklang::ik_f32>(0.35));
			game.m_soundEngine->play2D("assets/Sound/closeDoor.wav");

			lastTime = currentTime;
		}
		Game::m_doorHit = false;
	}

	// Plat footstep sound when walking every 1.35 second
	if (currentTime - lastTime > 1.35)
	{
		if (glfwGetKey(game.m_window.m_windowPtr, GLFW_KEY_W) == GLFW_PRESS)
			game.m_soundEngine->play2D("assets/Sound/walk.wav");

		if (glfwGetKey(game.m_window.m_windowPtr, GLFW_KEY_S) == GLFW_PRESS)
			game.m_soundEngine->play2D("assets/Sound/walk.wav");

		if (glfwGetKey(game.m_window.m_windowPtr, GLFW_KEY_A) == GLFW_PRESS)
			game.m_soundEngine->play2D("assets/Sound/walk.wav");

		if (glfwGetKey(game.m_window.m_windowPtr, GLFW_KEY_D) == GLFW_PRESS)
			game.m_soundEngine->play2D("assets/Sound/walk.wav");

		// Update last time a SFX was played
		lastTime = currentTime;
	}

}

void InitLevelOne(Game& game)
{
	// References for readability
	ResourceManager&	assets = game.m_currentLevel.m_assets;
	Graph<SceneNode>&	gameObjects = game.m_currentLevel.m_scene;
	BVHierarchy&		colliders = game.m_currentLevel.m_colliders;

	// Init sound engine
	InitSound(game);

	// Set sound volume to 25% and play BGM looped
	game.m_soundEngine->setSoundVolume(0.25);
	game.m_soundEngine->play2D("assets/Sound/game.wav", true);

	// Init player object
	Player::InitPlayer(gameObjects);

	// Create world bounding box to serve as a universal parent collider
	BoxBV*		world = colliders.AddCollider<BoxBV>("world");

	// Make it big enough to contain the entire scene
	world->m_boxScale = { 200.f, 200.f, 200.f };

	// Create bounding volumes for broad phase sweeping
	InitColliders(colliders);

	// Place objects into scene
	InitLevelOneHierarchies(game);

	// Open, read compile, link and save shaders
	Shader*		shader = assets.Create<Shader>("lighting shader");

	shader->InitShader("lighting.vert", "lighting.frag");

	Shader*		shader2 = assets.Create<Shader>("phone shader");

	shader2->InitShader("phone.vert", "phone.frag");

	Shader*		shader3 = assets.Create<Shader>("phone shader2");

	shader3->InitShader("phoneI.vert", "phoneI.frag");

	// Create player spot light
	SpotLight*		spot = gameObjects.AddChild<SpotLight>("spot");

	// Set light components
	spot->SetAmbient(WHITE, 1.0f);
	spot->SetDiffuse(WHITE, 1.0f);
	spot->SetSpecular(WHITE, 1.0f);

	spot->SetPosition({ 0.0f, 2.0f, 1.0f });
	spot->SetAttenuation({1.f, 0.09f, 0.032f });

	spot->SetCutoff(cos(LibMath::Degree(7.5f)), cos(LibMath::Degree(17.5f)));

	game.m_sceneFile.close();

}

void InitColliders(BVHierarchy& colliders)
{
	// Underground bounding box, no collisions withing this box will be tested if player is not inside it
	colliders.AddCollider<BoxBV>("world", std::string("underground"), LibMath::Vector3{ 25.1362f, -5.f, -7.19665f }, LibMath::Vector3{ 100.f, 5.f, 100.f });

	// Ground level, no collision testing within it if player is on top of the stairs or underground
	colliders.AddCollider<BoxBV>("world", std::string("ground level"), LibMath::Vector3{ 0.f, 1.5f, 0.f }, LibMath::Vector3{ 100.f, 1.5f, 100.f });

	// Second level, top of the stairs
	colliders.AddCollider<BoxBV>("world", std::string("second level"), LibMath::Vector3{ 0.f, 5.9f, 0.f }, LibMath::Vector3{ 100.f, 3.5f, 100.f });

	// Starting area on ground level, positive x z coordinates
	colliders.AddCollider<BoxBV>("ground level", std::string("start"), LibMath::Vector3{ 25.f, 1.5f, 25.f }, LibMath::Vector3{ 80.f, 1.5f, 80.f });

	// Area two on ground level, negative x and positive z
	colliders.AddCollider<BoxBV>("ground level", std::string("area2"), LibMath::Vector3{ -25.f, -0.3f, 25.f }, LibMath::Vector3{ 50.f, 10.f, 60.f });

	// Area three on ground level, negative x and z
	colliders.AddCollider<BoxBV>("ground level", std::string("area3"), LibMath::Vector3{ -25.f, -0.3f, -25.f }, LibMath::Vector3{ 35.f, 10.f, 35.f });

	// Area four on ground level, positve x and negative z
	colliders.AddCollider<BoxBV>("ground level", std::string("area4"), LibMath::Vector3{ 25.f, -0.3f, -25.f }, LibMath::Vector3{ 35.f, 10.f, 35.f });

	// Area one on second level (top of the stairs), same rules as ground level start
	colliders.AddCollider<BoxBV>("second level", std::string("areasec1"), LibMath::Vector3{ 25.f, 5.9f, 25.f }, LibMath::Vector3{ 35.f, 10.f, 35.f });

	// Area two on second level (top of the stairs), same rules as ground level area 2
	colliders.AddCollider<BoxBV>("second level", std::string("areasec2"), LibMath::Vector3{ -25.f, 5.9f, 25.f }, LibMath::Vector3{ 35.f, 10.f, 35.f });

	// Area three on second level (top of the stairs), same rules as ground level area 3
	colliders.AddCollider<BoxBV>("second level", std::string("areasec3"), LibMath::Vector3{ -25.f, 5.9f, -25.f }, LibMath::Vector3{ 35.f, 10.f, 35.f });

	// Area four on second level (top of the stairs), same rules as ground level area 4
	colliders.AddCollider<BoxBV>("second level", std::string("areasec4"), LibMath::Vector3{ 25.f, 5.9f, -25.f }, LibMath::Vector3{ 35.f, 10.f, 35.f });

	// Final tower going from second level to deep underground, only contains objects within said tower in-game
	colliders.AddCollider<BoxBV>("world", std::string("final tower"), LibMath::Vector3{ 9.f, 0.f, 23.f }, LibMath::Vector3{ 6.f, 20.f, 6.f }, PhysicsLib::FINAL_TOWER);
}

void InitLevelOneHierarchies(Game& game)
{
	// References for readability
	ResourceManager& assets = game.m_currentLevel.m_assets;
	Graph<SceneNode>& gameObjects = game.m_currentLevel.m_scene;

	// Floor model
	Model* floorModel = assets.Create<Model>("TriFloor.obj");

	// Wall models
	Model* wallModel = assets.Create<Model>("wallV10R.obj");
	Model* wallModel2 = assets.Create<Model>("wallV10.obj");

	// Wall models with holes
	Model* wallModelHole = assets.Create<Model>("wallV10RHole.obj");
	Model* wallModel2Hole = assets.Create<Model>("wallV10Hole.obj");

	// Load wall model with a hole on the side
	assets.Create<Model>("wallV10HoleSide.obj");

	// Set wall textures
	wallModel->m_texture = assets.Create<Texture>("wallTexture.png");;
	wallModelHole->m_texture = assets.Create<Texture>("WallHoleTexture.png");
	wallModel2->m_texture = wallModel->m_texture;
	wallModel2Hole->m_texture = wallModelHole->m_texture;

	// Load phone models
	Model* phoneModel = assets.Create<Model>("newPhone.obj");
	Model* phoneModel1 = assets.Create<Model>("newPhoneI.obj");

	// Load light models
	assets.Create<Model>("Light.obj");
	assets.Create<Model>("LightEdge.obj");

	// Load cube ,
	assets.Create<Model>("cube.obj");

	// Create material objects
	Material*		darkGray = assets.Create<Material>("dark", LibMath::Vector4{DARK_GRAY, 1.0f}, 89.2f);
	Material*		white = assets.Create<Material>("white", LibMath::Vector4{ WHITE, 1.0f }, 89.2f);

	assets.Create<Material>("light", LibMath::Vector4{LIGHT_GRAY , 1.f }, 750.f);


	// Place walls and floors
	//InitStartingAreaWalls(game);
	//InitAreaTwoWalls(game);
	//InitAreaThreeWalls(game);
	//InitAreaFourWalls(game);
	//InitUnderground(game);

	game.ReadSceneFile();

	// Create phone meshes
	Mesh*		phone = gameObjects.AddChild<Mesh>("player", std::string("phone"), phoneModel);
	Mesh*		phone1 = gameObjects.AddChild<Mesh>("phone", std::string("phone1"), phoneModel1);

	// Make phone invisible from the main shader
	phone->Scale({ 0.f, 0.f, 0.f });

	// Place lights and colored cubes
	PlaceLights(game);
	PlaceColorCubes(game);

	// Place doors
	InitDoors(game);

	// Link phone to scene node
	phone->LinkToNode(gameObjects.GetNode("phone"));
	phone1->LinkToNode(gameObjects.GetNode("phone1"));


	// Create teleporter
	game.CreateTeleporters("teleporter",
						 { 20.f, 1.476f, 46.f },	// ground level destination
					{ 30.f, -4.5f, 20.f });		// underground starting point
}


void InitDoors(Game& game)
{

	// References for readability
	ResourceManager&	assets = game.m_currentLevel.m_assets;

	// Get models from resoource manager
	Model*		doorModel = assets.Get<Model>("cube.obj");
	Model*		doorModelR = doorModel;
	Model*		doorModelF = assets.Get<Model>("TriFloor.obj");

	// Color objects to create doors
	Color		redColor = { RED, 1.f };
	Color		greenColor = { GREEN, 1.f };
	Color		whiteColor = { WHITE, 1.f };
	Color		blueColor = { BLUE, 1.f };
	Color		orangeColor = { ORANGE, 1.f };

	// Place doors
	game.CreateDoor("door1", { 35.f, 2.7f, 27.55f }, { 0.1f, 3.f, 2.5f }, redColor, doorModelR, "start");
	game.CreateDoor("door2", { 42.4f, 2.7f, -5.2f }, { 3.5f, 3.f, 0.1f }, whiteColor, doorModel, "area4");

	game.CreateDoor("door7", { 2.8f, 2.7f, -24.9f }, { 2.2f, 3.f, 0.1f }, redColor, doorModel, "area4");
	game.CreateDoor("door6", { -15.2f, 2.7f, -0.05f }, { 0.1f, 3.f, 3.1f }, whiteColor, doorModelR, "area3");
	game.CreateDoor("door5", { 0.f, 2.7f, 0.f }, { 0.1f, 3.f, 3.1f }, redColor, doorModelR, "world");
	game.CreateDoor("door4", { 10.1f, 2.7f, 0.f }, { 0.1f, 3.f, 3.1f }, whiteColor, doorModelR, "world");
	game.CreateDoor("door3", { 24.8f, 2.7f, -2.5f }, { 0.1f, 3.f, 2.5f }, redColor, doorModelR, "area4");
	game.CreateDoor("door8", { -25.4f, 2.7f, 9.4f }, { 0.1f, 3.f, 2.f }, whiteColor, doorModelR, "area2");
	game.CreateDoor("door9", { -25.4f, 2.7f, 43.f }, { 0.1f, 3.f, 3.f }, whiteColor, doorModelR, "area2");
	game.CreateDoor("door10", { -25.4f, 2.7f, 55.f }, { 0.1f, 3.f, 3.f }, greenColor, doorModelR, "area2");


	game.CreateDoor("door12", { -42.f, 2.7f, 5.f }, { 3.f, 3.f, 0.1f }, whiteColor, doorModel, "area2");
	game.CreateDoor("door13", { -10.f, -3.5f, 4.f }, { 5.f, 3.f, 0.1f }, orangeColor, doorModel, "underground");
	game.CreateDoor("door14", { 7.f, -3.5f, 5.4f }, { 5.f, 3.f, 0.1f }, blueColor, doorModel, "underground");
	game.CreateDoor("door15", { -15.f, 2.7f, 67.f }, { 0.1f, 3.f, 5.f }, orangeColor, doorModelR, "area2");
	game.CreateDoor("door16", { -13.f, 2.7f, 77.f }, { 0.1f, 3.f, 5.f }, whiteColor, doorModelR, "area2");

	game.CreateDoor("door17", { 30.f, -3.5f, 14.f }, { 5.f, 3.f, 0.1f }, orangeColor, doorModel, "underground");
	game.CreateDoor("door18", { 15.f, 2.7f, 45.f }, { 0.1f, 3.f, 5.1f }, orangeColor, doorModel, "start");

	// Create floor doors
	game.CreateDoor("doorF1", { 9.8f, -6.3f, -10.f }, { 5.f, 1.f, 7.f }, blueColor, doorModelF, "underground");

	game.CreateDoor("doorF2", { 24.9f, -6.3f, -22.2f }, { 10.f, 1.f, 5.f }, greenColor, doorModelF, "underground");
	game.CreateDoor("doorF3", { 40.1f, -6.3f, -7.1f }, { 5.f, 1.f, 10.f }, blueColor, doorModelF, "underground");
	game.CreateDoor("doorF4", { 30.f, -6.3f, 8.f }, { 5.f, 1.f, 5.f }, whiteColor, doorModelF, "underground");
	game.CreateDoor("doorF5", { 9.7f, -12.3f, 23.1f }, { 5.f, 1.f, 7.f }, redColor, doorModelF, "world");
	game.CreateDoor("doorF6", { 9.7f, -6.3f, 23.1f }, { 5.f, 1.f, 7.f }, blueColor, doorModelF, "underground");

	game.CreateDoor("doorF7", { 9.7f, -0.3f, 23.1f }, { 5.f, 1.f, 7.f }, greenColor, doorModelF, "world");

	game.CreateDoor("doorF8", { 9.7f, 5.7f, 23.1f }, { 5.f, 1.f, 7.f }, whiteColor, doorModelF, "areasec1");
}



void UpdateLevelOne(Game& game)
{
	// Update game sound engine
	UpdateSound(game);

	// References for readability
	Graph<SceneNode>&	gameObjects = game.m_currentLevel.m_scene;
	BVHierarchy&		colliders = game.m_currentLevel.m_colliders;

	// Update scene hierarchy
	gameObjects.UpdateGraph();

	// Get camera from graph
	Camera*		camera = GetObject<Camera>(gameObjects, "camera");

	// Update camera view and projection matrices
	camera->UpdateCamera(game.m_window);

	// Create frustum from camera
	Frustum		view = camera->CameraFrustum(game.m_window.GetAspectRatio(), 0.1f, 100.0f);

	// Get player from scene graph
	Player*		player = GetObject<Player>(gameObjects, "player");

	// Update collider hierarchy
	colliders.Update(view);

	// Update player logic
	player->UpdatePlayer(game.m_window, static_cast<float>(game.m_timer.GetDeltaTime()), colliders);

	// Get spot light from graph
	SpotLight*	spot = GetObject<SpotLight>(gameObjects, "spot");

	// Turn on spot light
	spot->m_enabled = true;

	// Render scene
	DrawScene(game);

	// Get background window
	ImDrawList*			draw = ImGui::GetBackgroundDrawList();

	LibMath::Vector2	screenSize = game.m_window.GetWindowSize();

	// Draw crosshair if cursor is locked onto screen
	if (Application::m_lockCursor)
		draw->AddCircle(ImVec2(screenSize.m_x / 2, screenSize.m_y / 2), screenSize.m_y / 200, IM_COL32(255, 255, 255, 255), 10, 1.0f);
	else
		game.m_currentState = PAUSE;


	if (Game::m_gameOver)
	{
		Game::m_gameOver = false;

		game.ResetLevel();
		InitLevelOne(game);
		game.m_currentState = IN_GAME;
	}

	else if (player->m_position.m_y <= -15.f && player->m_isGrounded)
		game.m_currentState = GAME_OVER;

	else if (player->m_insideTower)
		game.m_menu.DisplayMessage("Press shift if you find yourself stuck here", 77);

}

void DrawScene(Game& game)
{
	LibMath::Vector2 screenSize = game.m_window.GetWindowSize();

	ResourceManager& assets = game.m_currentLevel.m_assets;
	Graph<SceneNode>& gameObjects = game.m_currentLevel.m_scene;

	Shader* shader = assets.Get<Shader>("lighting shader");
	Shader* shader2 = assets.Get<Shader>("phone shader");
	Shader* shader3 = assets.Get<Shader>("phone shader2");

	Mesh* phone = GetObject<Mesh>(gameObjects, "phone");
	Mesh* phone1 = GetObject<Mesh>(gameObjects, "phone1");

	Camera* camera = GetObject<Camera>(gameObjects, "camera");

	SpotLight* spot = GetObject<SpotLight>(gameObjects, "spot");
	spot->SetPosition(camera->m_position);

	LibMath::Vector3 direction = camera->GetFrontVector();
	spot->SetDirection(direction.m_x, direction.m_y, direction.m_z);

	game.m_window.SetWindowColor({ 0.f, 0.f, 0.f, 1.f }, RENDER_MODE::FILL);
	ParseScene(gameObjects, shader, camera, game.m_timer.GetDeltaTime());


	shader2->Use();

	if (Door::m_playerColor)
		shader2->SetUniform("Color", LibMath::Vector3(*Door::m_playerColor));
	else
		shader->SetUniform("Color", { 1.f, 1.f, 1.f });

	phone1->Draw(*shader2);

	shader3->Use();
	shader3->SetUniform("Color", { 0.f, 0.f, 0.f });
	phone->Draw(*shader3);

	ResetLights();

	game.m_timer.SetNewFrame(true);
}