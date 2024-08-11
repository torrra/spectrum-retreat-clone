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

	game.m_sceneFile.open("scenes/levelOne.level", std::ios::in | std::ios::binary);

	if (!game.m_sceneFile)
		__debugbreak();

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


	// Create ceiling meshes
	game.CreateBlock("ceil1", floorModel, "TriFloor.obj", { -10.2f, 5.9f, -13.9f }, { 60.f, 1.f, 30.f }, white, "white", "world");
	game.CreateBlock("ceil2", floorModel, "TriFloor.obj", { 38.7f, 5.9f, 35.f }, { 15.f, 1.f, 20.f }, white, "white", "areasec1");
	game.CreateBlock("ceil3", floorModel, "TriFloor.obj", { -16.f, 5.9f, 50.f }, { 40.f, 1.f, 20.f }, white, "white", "areasec2");
	game.CreateBlock("ceil4", floorModel, "TriFloor.obj", { -25.f, 5.9f, 23.f }, { 29.7f, 1.f, 7.f }, white, "white", "areasec2");
	game.CreateBlock("ceil5", floorModel, "TriFloor.obj", { 16.f, 11.9f, 16.f }, { 15.f, 1.f, 15.f }, white, "white", "world");
	game.CreateBlock("ceil6", floorModel, "TriFloor.obj", { 8.f, 5.9f, 74.f }, { 35.f, 1.f, 19.f }, white, "white", "areasec1");
	game.CreateBlock("ceil7", floorModel, "TriFloor.obj", { 15.f, 5.9f, -43.f }, { 35.f, 1.f, 5.f }, white, "white", "areasec4");


	// Create stairs meshes
	game.CreateBlock("stair1", floorModel, "TriFloor.obj", { 19.4f, 5.f, 17.f }, { 5.f, 1.f, 1.f }, white, "white", "world");
	game.CreateBlock("stair2", floorModel, "TriFloor.obj", { 19.4f, 4.f, 20.f }, { 5.f, 1.f, 1.f }, white, "white", "world");
	game.CreateBlock("stair3", floorModel, "TriFloor.obj", { 19.4f, 3.f, 23.f }, { 5.f, 1.f, 1.f }, white, "white", "world");
	game.CreateBlock("stair4", floorModel, "TriFloor.obj", { 19.4f, 2.f, 26.f }, { 5.f, 1.f, 1.f }, white, "white", "world");
	game.CreateBlock("stair5", floorModel, "TriFloor.obj", { 19.4f, 1.f, 29.f }, { 5.f, 1.f, 1.f }, white, "white", "world");


	// Place walls and floors
	InitStartingAreaWalls(game);
	InitAreaTwoWalls(game);
	InitAreaThreeWalls(game);
	InitAreaFourWalls(game);
	InitUnderground(game);

	//game.ReadSceneFile();

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


void InitStartingAreaWalls(Game& game)
{
	// Reference for readability
	ResourceManager&	assets = game.m_currentLevel.m_assets;

	// Get models from resource manager
	Model*			floorModel = assets.Get<Model>("TriFloor.obj");
	Model*			wallModel = assets.Get<Model>("wallV10R.obj");
	Model*			wallModel2 = assets.Get<Model>("wallV10.obj");
	Model*			wallModelHole = assets.Get<Model>("wallV10RHole.obj");
	Model*			wallModel2Hole = assets.Get<Model>("wallV10Hole.obj");

	Material*		white = assets.Get<Material>("white");
	Material*		lightGray = assets.Get<Material>("light");

	const char*		areaKey = "start";

	// Wall scales
	const LibMath::Vector3		scale{ 1.f,  3.f, 5.f };
	const LibMath::Vector3		scaleR{ 5.f,  3.f, 1.f };

	// Create walls with holes
	game.CreateWallWithHole("wall10", wallModel2Hole, "wallV10Hole.obj", { 35.f,  2.7f, 20.0f }, scale, lightGray, "light", areaKey);
	game.CreateWallWithHole("wall10a", wallModelHole, "wallV10RHole.obj", { 30.f,  2.7f, 15.0f }, scaleR, lightGray, "light", areaKey);
	game.CreateWallWithHole("wall10b", wallModelHole, "wallV10RHole.obj", { -10.f,  2.7f, -10.0f }, { 5.f,  1.f,  1.0f }, lightGray, "light", areaKey);
	game.CreateWallWithHole("wall17", wallModelHole, "wallV10RHole.obj", { 5.f,  2.7f, 5.f }, scaleR, lightGray, "light", areaKey);

	// Place 'a few' more walls
	game.CreateBlock("wall5", wallModel, "wallV10R.obj", { 45.f,  2.7f, 40.f }, scaleR, lightGray, "light", areaKey);
	game.CreateBlock("wall5b", wallModel, "wallV10R.obj", { 37.f,  2.7f, 40.f }, { 3.f, 3.f,  1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall6", wallModel2, "wallV10.obj", { 24.8f, 2.7f, 45.f },   scale, lightGray, "light", areaKey);
	game.CreateBlock("wall6b", wallModel2, "wallV10.obj", { 24.8f, 2.7f, 35.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall6c", wallModel2, "wallV10.obj", { 24.8f, 2.7f, 25.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall6d", wallModel2, "wallV10.obj", { 24.8f, 2.7f, 15.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall8", wallModel2, "wallV10.obj", { 35.f,  2.7f, 35.1f }, { 1.f,  3.f,  5.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall2", wallModel, "wallV10R.obj", { 45.f, 2.7f, 50.f }, scaleR, lightGray, "light", areaKey);
	game.CreateBlock("wall2a", wallModel, "wallV10R.obj", { 35.f, 2.7f, 50.f }, scaleR, lightGray, "light", areaKey);
	game.CreateBlock("wall2b", wallModel, "wallV10R.obj",  { 25.f, 2.7f, 50.f }, scaleR, lightGray, "light", areaKey);
	game.CreateBlock("wall2c", wallModel, "wallV10R.obj", { 15.f, 2.7f, 50.f }, scaleR, lightGray, "light", areaKey);

	game.CreateBlock("wall4", wallModel2, "wallV10.obj", { 50.f, 2.7f, 45.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall4b", wallModel2, "wallV10.obj", { 50.f, 2.7f, 35.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall4c", wallModel2, "wallV10.obj", { 50.f, 2.7f, 25.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall4d", wallModel2, "wallV10.obj", { 50.f, 2.7f, 15.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall4e", wallModel2, "wallV10.obj", { 50.f, 2.7f,  5.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall4f", wallModel2, "wallV10.obj", { 50.f, 2.7f,  5.f }, scale, lightGray, "light", areaKey);

	game.CreateBlock("wall12", wallModel2, "wallV10.obj", { 35.f,  2.7f, 10.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall12b", wallModel2, "wallV10.obj", { 35.f,  2.7f, 0.f }, scale, lightGray, "light", areaKey);

	game.CreateBlock("wall14", wallModel2, "wallV10.obj", { 24.8f, 2.7f, 5.f }, scale, lightGray, "light", areaKey);

	game.CreateBlock("wall16", wallModel, "wallV10R.obj", { 20.f,  2.7f, 15.f }, scaleR, lightGray, "light", areaKey);
	game.CreateBlock("wall16b", wallModel, "wallV10R.obj", { 10.f,  2.7f, 15.f }, scaleR, lightGray, "light", areaKey);
	game.CreateBlock("wall16c", wallModel, "wallV10R.obj", { 0.f,  2.7f, 15.f }, scaleR, lightGray, "light", areaKey);

	game.CreateBlock("wall28", wallModel2, "wallV10.obj", { 10.1f, 2.7f, 4.1f }, { 1.f,  3.f,  1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall45", wallModel, "wallV10R.obj", { 20.f, 2.7f, 39.5f }, { 5.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall46", wallModel, "wallV10R.obj", { 7.4f, 2.7f, 50.f }, { 2.6f, 3.f, 1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall67", wallModel2, "wallV10.obj", { 42.5f, 2.7f, 67.f }, { 1.f, 3.f, 15.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall68", wallModel, "wallV10R.obj", { 4.f, 2.7f, 50.f }, { 10.f, 3.f, 1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall70", wallModel, "wallV10R.obj", { 10.f, 2.7f, 82.f }, { 35.f, 3.f, 1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall72", wallModel, "wallV10R.obj", { 8.5f, 2.7f, 72.f }, { 24.5f, 3.f, 1.f }, lightGray, "light", "world");


	// Place floors
	game.CreateBlock("floor1", floorModel, "TriFloor.obj", { 37.5f, -0.3f, 45.f }, { 12.7f, 1.f, 4.9f }, white, "white", areaKey);
	game.CreateBlock("floor2", floorModel, "TriFloor.obj", { 29.9f, -0.3f, 27.6f }, { 5.1f, 1.f, 12.5f }, white, "white", areaKey);
	game.CreateBlock("floor3", floorModel, "TriFloor.obj", { 42.4f, -0.3f, 17.f }, { 7.5f, 1.f, 22.9f }, white, "white", areaKey);
	game.CreateBlock("floor5", floorModel, "TriFloor.obj", { 29.9f, -0.3f, 4.5f }, { 5.f, 1.f, 10.4f }, white, "white", areaKey);

	game.CreateBlock("floor22", floorModel, "TriFloor.obj", { 14.8f, -0.3f, 45.f }, { 10.f, 1.f, 5.f }, white, "white", areaKey);
	game.CreateBlock("floor23", floorModel, "TriFloor.obj", { 4.6f, -0.3f, 35.f }, { 20.f, 1.f, 5.f }, white, "white", "world");
	game.CreateBlock("floor24", floorModel, "TriFloor.obj", { 19.8f, -0.3f, 22.6f }, { 5.f, 1.f, 7.4f }, white, "white", areaKey);

	game.CreateBlock("floor33", floorModel, "TriFloor.obj", { 3.4f, -0.3f, 55.f }, { 3.f, 1.f, 5.f }, white, "white", areaKey);

	game.CreateBlock("ceil8", floorModel, "TriFloor.obj", { 52.f, -0.3f, 38.f }, { 20.f, 1.f, 5.f }, white, "white", areaKey);

	// Place upper level walls
	areaKey = "second level";
	game.CreateBlock("wall99", wallModel, "wallV10R.obj", { 14.f, 8.9f, 30.5f }, { 10.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall100", wallModel2, "wallV10.obj", { 4.5f, 8.9f, 17.f }, { 1.f, 3.f, 15.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall101", wallModel, "wallV10R.obj", { 14.f, 8.9f, 3.f }, { 10.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall102", wallModel2, "wallV10.obj", { 24.f, 8.9f, 17.f }, { 1.f, 3.f, 15.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall103", wallModel2, "wallV10.obj", { 15.f, 8.7f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray, "light", areaKey);
}


void InitAreaTwoWalls(Game& game)
{
	// Reference for readability
	ResourceManager&	assets = game.m_currentLevel.m_assets;

	// Get models from asset manager
	Model*		floorModel = assets.Get<Model>("TriFloor.obj");
	Model*		wallModel = assets.Get<Model>("wallV10R.obj");
	Model*		wallModel2 = assets.Get<Model>("wallV10.obj");

	Model*		wallModelHole = assets.Get<Model>("wallV10RHole.obj");
	Model*		wallModel2Hole = assets.Get<Model>("wallV10Hole.obj");

	// Get materials from asser manager
	Material*	white = assets.Get<Material>("white");
	Material*	lightGray = assets.Get<Material>("light");


	const char*		areaKey = "area2";

	// Wall scale
	const LibMath::Vector3 scaleR{ 5.f,  3.f, 1.f };

	// Place walls with holes
	game.CreateWallWithHole("wall17b", wallModelHole, "wallV10RHole.obj", { -5.f,  2.7f, 5.f }, scaleR, lightGray, "light", areaKey);
	game.CreateWallWithHole("wall81", wallModel2Hole, "wallV10Hole.obj", { -25.5f,  2.7f, 13.4f }, { 1.f, 3.f,  2.f }, lightGray, "light", areaKey);
	game.CreateWallWithHole("wall82", wallModelHole, "wallV10RHole.obj", { 20.f,  2.7f, -24.9f }, { 5.f, 3.f,  1.f }, lightGray, "light", areaKey);
	game.CreateWallWithHole("wall74", wallModelHole, "wallV10RHole.obj", { -34.f, 2.7f, 5.f }, scaleR, lightGray, "light", areaKey);


	// Place 'a couple' other walls
	game.CreateBlock("wall16d", wallModel, "wallV10R.obj", { -10.f,  2.7f, 15.f }, scaleR, lightGray, "light", areaKey);
	game.CreateBlock("wall17c", wallModel, "wallV10R.obj", { -15.f,  2.7f, 5.f }, scaleR, lightGray, "light", areaKey);

	game.CreateBlock("wall23", wallModel2, "wallV10.obj", { -15.2f, 2.7f,  4.1f }, { 1.f,  3.f,  1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall24", wallModel2, "wallV10.obj", { -24.1f, 2.7f,  0.f }, { 1.f,  3.f, 5.1f }, lightGray, "light", areaKey);
	game.CreateBlock("wall25", wallModel2, "wallV10.obj", { -15.2f, 2.7f,  12.5f }, { 1.f,  3.f, 7.5f }, lightGray, "light", areaKey);


	game.CreateBlock("wall47", wallModel, "wallV10R.obj", { -0.3f, 2.7f, 30.f }, { 15.2f, 3.f, 1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall48", wallModel, "wallV10R.obj", { -5.3f, 2.7f, 40.f }, { 10.1f, 3.f, 1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall50", wallModel2, "wallV10.obj", { 14.8f, 2.7f, 22.7f }, { 1.f, 3.f, 7.2f }, lightGray, "light");
	game.CreateBlock("wall51", wallModel2, "wallV10.obj", { 4.8f, 2.7f, 45.f }, { 1.f, 3.f, 5.1f }, lightGray, "light", areaKey);
	game.CreateBlock("wall52", wallModel2, "wallV10.obj", { 4.5f, 2.7f, 23.f }, { 1.f, 3.f, 7.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall53", wallModel2, "wallV10.obj", { -15.6f, 2.7f, 55.f }, { 1.f, 3.f, 7.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall54", wallModel2, "wallV10.obj", { -5.4f, 2.7f, 45.f }, { 1.f, 3.f, 5.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall55", wallModel2, "wallV10.obj", { -25.4f, 2.7f, 6.4f }, { 1.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall56", wallModel2, "wallV10.obj", { -25.4f, 2.7f, 23.f }, { 1.f, 3.f, 8.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall57", wallModel2, "wallV10.obj", { -25.4f, 2.7f, 39.f }, { 1.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall58", wallModel2, "wallV10.obj", { -25.4f, 2.7f, 49.f }, { 1.f, 3.f, 3.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall59", wallModel2, "wallV10.obj", { -25.4f, 2.7f, 63.f }, { 1.f, 3.f, 5.f }, lightGray, "light", areaKey);


	game.CreateBlock("wall59a", wallModel2, "wallV10.obj", { 14.5f, -3.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray, "light");
	game.CreateBlock("wall59b", wallModel2, "wallV10.obj", { 4.5f, -3.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray, "light");
	game.CreateBlock("wall59c", wallModel2, "wallV10.obj", { 14.5f, -9.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray, "light");
	game.CreateBlock("wall59d", wallModel2, "wallV10.obj", { 4.5f, -9.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray, "light");

	game.CreateBlock("wall59e", wallModel2, "wallV10.obj", { 14.5f, -15.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray, "light");
	game.CreateBlock("wall59f", wallModel2, "wallV10.obj", { 4.5f, -15.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray, "light");
	game.CreateBlock("wall59g", wallModel, "wallV10R.obj", { 9.5f, -3.3f, 30.f }, scaleR, lightGray, "light");
	game.CreateBlock("wall59i", wallModel, "wallV10R.obj", { 9.5f, -9.3f, 30.f }, scaleR, lightGray, "light");
	game.CreateBlock("wall59j", wallModel, "wallV10R.obj", { 9.5f, -9.3f, 16.f }, scaleR, lightGray, "light");
	game.CreateBlock("wall59k", wallModel, "wallV10R.obj", { 9.5f, -15.3f, 30.f }, scaleR, lightGray, "light");
	game.CreateBlock("wall59l", wallModel, "wallV10R.obj", { 9.5f, -15.3f, 16.f }, scaleR, lightGray, "light");

	game.CreateBlock("wall60", wallModel2, "wallV10.obj", { -32.4f, 2.7f, 48.f }, { 1.f, 3.f, 10.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall61", wallModel2, "wallV10.obj", { -40.f, 2.7f, 44.f }, { 1.f, 3.f, 6.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall62", wallModel2, "wallV10.obj", { -47.f, 2.7f, 52.f }, { 1.f, 3.f, 6.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall64", wallModel, "wallV10R.obj", { -40.f, 2.7f, 38.f }, { 15.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall65", wallModel, "wallV10R.obj", { -40.f, 2.7f, 68.f }, { 15.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall66", wallModel, "wallV10R.obj", { -40.f, 2.7f, 58.f }, { 7.f, 3.f, 1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall69", wallModel2, "wallV10.obj", { -25.f, 2.7f, 75.f }, { 1.f, 3.f, 7.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall73", wallModel, "wallV10R.obj", { -10.7f, 2.7f, 62.f }, { 5.3f, 3.f, 1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall75", wallModel, "wallV10R.obj", { -35.7f, 2.7f, 15.f }, { 10.f, 3.f, 1.f }, lightGray, "light", areaKey);


	game.CreateBlock("wall95", wallModel, "wallV10R.obj", { -30.5f, 2.7f, 28.f }, { 5.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall96", wallModel2, "wallV10.obj", { -34.5f, 2.7f, 33.f }, { 1.f, 3.f, 5.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall97", wallModel, "wallV10R.obj", { -10.f, 2.7f, 50.f }, { 5.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall98", wallModel2, "wallV10.obj", { -15.6f, 2.7f, 44.4f }, { 1.f, 3.f, 5.f }, lightGray, "light", areaKey);


	// Create floors
	game.CreateBlock("floor32", floorModel, "TriFloor.obj", { -8.4f, -0.3f, 67.f }, { 7.f, 1.f, 5.f }, white, "white", areaKey);

	game.CreateBlock("floor34", floorModel, "TriFloor.obj", { 13.4f, -0.3f, 67.f }, { 5.f, 1.f, 5.f }, white, "white", "world");
	game.CreateBlock("floor35", floorModel, "TriFloor.obj", { 25.4f, -0.3f, 55.f }, { 5.f, 1.f, 5.f }, white, "white", "world");
	game.CreateBlock("floor36", floorModel, "TriFloor.obj", { 37.4f, -0.3f, 67.f }, { 5.f, 1.f, 5.f }, white, "white", "world");
	game.CreateBlock("floor37", floorModel, "TriFloor.obj", { 9.f, -0.3f, 77.f }, { 34.f, 1.f, 5.f }, white, "white", "world");
	game.CreateBlock("floor38", floorModel, "TriFloor.obj", { -20.4f, -0.3f, 67.f }, { 5.f, 1.f, 6.f }, white, "white", areaKey);
	game.CreateBlock("floor39", floorModel, "TriFloor.obj", { 9.7f, -18.3f, 23.1f }, { 5.f, 1.f, 7.f }, white, "white", "world");
	game.CreateBlock("floor40", floorModel, "TriFloor.obj", { -35.4f, -0.3f, 10.f }, { 10.f, 1.f, 5.f }, white, "white", areaKey);

	game.CreateBlock("floor8", floorModel, "TriFloor.obj", { -8.5f, -0.3f, 10.f }, { 6.6f, 1.f, 5.f }, white, "white", areaKey);
	game.CreateBlock("floor25", floorModel, "TriFloor.obj", { -20.4f, -0.3f, 35.f }, { 5.f, 1.f, 30.f }, white, "white", areaKey);
	game.CreateBlock("floor26", floorModel, "TriFloor.obj", { -10.4f, -0.3f, 45.f }, { 5.f, 1.f, 5.f }, white, "white", areaKey);
	game.CreateBlock("floor27", floorModel, "TriFloor.obj", { -30.4f, -0.3f, 33.f }, { 5.f, 1.f, 5.f }, white, "white", areaKey);
	game.CreateBlock("floor28", floorModel, "TriFloor.obj", { -45.4f, -0.3f, 53.f }, { 20.f, 1.f, 15.f }, white, "white", areaKey);
}

void InitAreaThreeWalls(Game& game)
{
	// Reference for readability
	ResourceManager&		assets = game.m_currentLevel.m_assets;

	// Get models from asset manager
	Model*			floorModel = assets.Get<Model>("TriFloor.obj");
	Model*			wallModel = assets.Get<Model>("wallV10R.obj");
	Model*			wallModel2 = assets.Get<Model>("wallV10.obj");

	// Get materials from assets manager
	Material*		white = assets.Get<Material>("white");
	Material*		lightGray = assets.Get<Material>("light");

	const char*		areaKey = "area3";

	// Wall scale
	const LibMath::Vector3		scaleR{ 5.f,  3.f, 1.f };

	// Create walls
	game.CreateBlock("wall18c", wallModel, "wallV10R.obj", { -10.f,  2.7f,-5.f }, scaleR, lightGray, "light", areaKey);
	game.CreateBlock("wall18d", wallModel, "wallV10R.obj", { -20.f,  2.7f,-5.f }, scaleR, lightGray, "light", areaKey);

	game.CreateBlock("wall20", wallModel, "wallV10R.obj", { -1.f,  2.7f,-24.9f }, { 1.f,  3.f,  1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall22", wallModel2, "wallV10.obj", { -15.2f, 2.7f,-4.1f }, { 1.f,  3.f,  1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall76", wallModel, "wallV10R.obj", { -35.7f, 2.7f, -15.f }, { 10.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall77", wallModel2, "wallV10.obj", { -25.5f, 2.7f, -5.f }, { 1.f, 3.f, 10.f }, lightGray, "light", areaKey);


	// Place floor pieces
	game.CreateBlock("floor29", floorModel, "TriFloor.obj", { -20.f, -0.3f, -31.f }, { 5.f, 1.f, 4.f }, white, "white", areaKey);
	game.CreateBlock("floor30", floorModel, "TriFloor.obj", { -6.f, -0.3f, -20.f }, { 9.f, 1.f, 19.1f }, white, "white", areaKey);
	game.CreateBlock("floor41", floorModel, "TriFloor.obj", { -35.4f, -0.3f, -5.f }, { 10.f, 1.f, 10.f }, white, "white", areaKey);
}

void InitAreaFourWalls(Game& game)
{
	ResourceManager&	assets = game.m_currentLevel.m_assets;

	// Get models from resource manager
	Model*			floorModel = assets.Get<Model>("TriFloor.obj");
	Model*			wallModel = assets.Get<Model>("wallV10R.obj");
	Model*			wallModel2 = assets.Get<Model>("wallV10.obj");

	// Get materials from resource manager
	Material*		white = assets.Get<Material>("white");
	Material*		lightGray = assets.Get<Material>("light");

	const char*		areaKey = "area4";

	// Wall scale
	const LibMath::Vector3	scale{ 1.f,  3.f, 5.f };


	// Place walls
	game.CreateBlock("wall1", wallModel, "wallV10R.obj", { 0.f, 2.7f,-45.f }, { 50.f, 3.f, 1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall3", wallModel2, "wallV10.obj", { -55.f, 2.7f,  10.f }, { 1.f,  3.f, 60.f }, lightGray, "light", "world");

	game.CreateBlock("wall4g", wallModel2, "wallV10.obj", { 50.f, 2.7f, -5.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall4h", wallModel2, "wallV10.obj", { 50.f, 2.7f,-15.f }, scale, lightGray, "light", areaKey);


	game.CreateBlock("wall9", wallModel, "wallV10R.obj", { 36.9f, 2.7f, -5.2f }, { 2.f,  3.f,  1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall11", wallModel, "wallV10R.obj", { 47.9f, 2.7f, -5.2f }, { 2.f,  3.f,  1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall13", wallModel, "wallV10R.obj", { 45.f,  2.7f,-15.f }, { 5.f,3.f,  1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall13b", wallModel, "wallV10R.obj", { 35.f,  2.7f,-15.f }, { 5.f,3.f,  1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall13c", wallModel, "wallV10R.obj", { 28.f,  2.7f,-15.f }, { 2.5f,3.f,  1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall15", wallModel2, "wallV10.obj", { 24.8f, 2.7f,-10.f }, { 1.f,  3.f,  5.f }, lightGray, "light", areaKey);


	game.CreateBlock("wall17d", wallModel, "wallV10R.obj", { -25.f,  2.7f, 5.f }, { 5.f, 3.f,  1.f }, lightGray, "light", "world");
	game.CreateBlock("wall18", wallModel, "wallV10R.obj", { 10.f,  2.7f,-5.f }, { 5.f, 3.f,  1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall18b", wallModel, "wallV10R.obj", { 0.f,  2.7f,-5.f }, { 5.f, 3.f,  1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall19", wallModel, "wallV10R.obj", { 10.f,  2.7f,-24.9f }, { 5.f, 3.f,  1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall21", wallModel, "wallV10R.obj", { 11.f,  2.7f,-45.f }, { 14.f, 3.f,  1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall26", wallModel2, "wallV10.obj", { 24.8f, 2.7f,-20.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall26b", wallModel2, "wallV10.obj", { 24.8f, 2.7f,-30.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall26c", wallModel2, "wallV10.obj", { 24.8f, 2.7f,-40.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall27", wallModel2, "wallV10.obj", { 0.f,  2.7f,-10.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall27b", wallModel2, "wallV10.obj", { 0.f,  2.7f,-20.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall27c", wallModel2, "wallV10.obj", { 0.f,  2.7f,-30.f }, scale, lightGray, "light", areaKey);
	game.CreateBlock("wall27d", wallModel2, "wallV10.obj", { 0.f,  2.7f,-40.f }, scale, lightGray, "light", areaKey);

	game.CreateBlock("wall29", wallModel2, "wallV10.obj", { 10.1f, 2.7f,-4.1f }, { 1.f,  3.f,  1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall30", wallModel2, "wallV10.obj", { 0.f,  2.7f, 4.1f }, { 1.f,  3.f,  1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall31", wallModel2, "wallV10.obj", { 0.f,  2.7f,-4.1f }, { 1.f,  3.f,  1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall78", wallModel2, "wallV10.obj", { -45.f, 2.7f, 0.f }, { 1.f, 3.f, 15.f }, lightGray, "light", "world");


	// PLace floors
	game.CreateBlock("floor4", floorModel, "TriFloor.obj", { 37.5f, -0.3f, -10.4f }, { 12.6f, 1.f, 4.5f }, white, "white", areaKey);
	game.CreateBlock("floor6", floorModel, "TriFloor.obj", { 11.5f, -0.3f, -4.9f }, { 13.4f, 1.f, 19.9f }, white, "white", "world");
	game.CreateBlock("floor7", floorModel, "TriFloor.obj", { 11.4f, -0.3f, -34.9f }, { 13.3f, 1.f, 10.1f }, white, "white", areaKey);
	game.CreateBlock("floor9", floorModel, "TriFloor.obj", { -8.6f, -0.3f, 0.f }, { 6.7f, 1.f, 5.f }, white, "white", "world");
	game.CreateBlock("floor10", floorModel, "TriFloor.obj", { -19.6f, -6.3f, 0.f }, { 4.4f, 1.f, 5.f }, white, "white", "world");

	game.CreateBlock("floor31", floorModel, "TriFloor.obj", { 45.f, -0.3f, -30.f }, { 21.f, 1.f, 20.f }, white, "white", areaKey);

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


void InitUnderground(Game& game)
{
	// References for readability
	ResourceManager&	assets = game.m_currentLevel.m_assets;

	// Get models from resource manager
	Model*			floorModel = assets.Get<Model>("TriFloor.obj");
	Model*			wallModel = assets.Get<Model>("wallV10R.obj");
	Model*			wallModel2 = assets.Get<Model>("wallV10.obj");
	Model*			wallModelHole = assets.Get<Model>("wallV10RHole.obj");
	Model*			wallModel2Hole = assets.Get<Model>("wallV10Hole.obj");

	// Get materials from resource manager
	Material*		white = assets.Get<Material>("white");
	Material*		lightGray = assets.Get<Material>("light");

	const char*		areaKey = "underground";

	// Place walls
	game.CreateBlock("wall32", wallModel, "wallV10R.obj", { -19.7f, -3.5f, -5.f }, { 4.5f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall33", wallModel2, "wallV10.obj", { -24.1f, -3.5f, 0.f }, { 1.f, 3.f, 5.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall34", wallModel, "wallV10R.obj", { -19.7f, -3.5f, 5.f }, { 4.5f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall35", wallModel2, "wallV10.obj", { -15.3f, -3.5f, -16.f }, { 1.f, 3.f, 11.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall36", wallModel2, "wallV10.obj", { -15.3f, -3.5f, 9.f }, { 1.f, 3.f, 6.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall37", wallModel2, "wallV10.obj", { -5.1f, -3.5f, -15.f }, { 1.f, 3.f, 20.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall38", wallModel, "wallV10R.obj", { -15.2f, -3.5f, -35.f }, { 10.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall39", wallModel, "wallV10R.obj", { -20.2f, -3.5f, -27.f }, { 5.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall40", wallModel, "wallV10R.obj", { -0.2f, -3.5f, 15.f }, { 15.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall42", wallModel2, "wallV10.obj", { -25.1f, -3.5f, -31.f }, { 1.f, 3.f, 3.9f }, lightGray, "light", areaKey);

	game.CreateBlock("wall83", wallModel, "wallV10R.obj", { 0.f, -3.5f, 5.2f }, { 5.f, 3.f, 1.f }, lightGray, "light", "world");

	game.CreateBlock("wall86", wallModel, "wallV10R.obj", { 25.f, -3.5f, -12.8f }, { 3.3f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall87", wallModel, "wallV10R.obj", { 25.f, -3.5f, -1.5f }, { 3.3f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall88", wallModel2, "wallV10.obj", { 28.5f, -3.5f, -7.f }, { 1.f, 3.f, 5.5f }, lightGray, "light", areaKey);
	game.CreateBlock("wall89", wallModel2, "wallV10.obj", { 21.5f, -3.5f, -7.f }, { 1.f, 3.f, 5.5f }, lightGray, "light", areaKey);


	game.CreateBlock("wall104", wallModel, "wallV10R.obj", { 23.f, -3.5f, -35.f }, { 30.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall105", wallModel2, "wallV10.obj", { 50.f, -3.5f, -2.f }, { 1.f, 3.f, 35.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall106", wallModel, "wallV10R.obj", { 15.f, -3.5f, 6.f }, { 3.f, 3.f, 1.f }, lightGray, "light", areaKey);

	game.CreateBlock("wall90", wallModel, "wallV10R.obj", { 45.f, -3.5f, 33.f }, { 10.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall91", wallModel, "wallV10R.obj", { 40.f, -3.5f, 43.f }, { 15.f, 3.f, 1.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall92", wallModel2, "wallV10.obj", { 35.f, -3.5f, 23.f }, { 1.f, 3.f, 10.5f }, lightGray, "light", areaKey);
	game.CreateBlock("wall93", wallModel2, "wallV10.obj", { 25.5f, -3.5f, 28.5f }, { 1.f, 3.f, 16.f }, lightGray, "light", areaKey);
	game.CreateBlock("wall94", wallModel2, "wallV10.obj", { 54.f, -3.5f, 38.f }, { 1.f, 3.f, 5.5f }, lightGray, "light", areaKey);

	// Place wall with a hole
	game.CreateWallWithHole("wall85", wallModel2Hole, "wallV10Hole.obj", { 12.5f, -3.5f, 10.f }, { 1.f, 3.f, 5.f }, lightGray, "light", areaKey);

	// Place floor tiles
	game.CreateBlock("floor11", floorModel, "TriFloor.obj", { -10.2f, -6.3f, -7.f }, { 5.f, 1.f, 22.f }, white, "white", areaKey);
	game.CreateBlock("floor12", floorModel, "TriFloor.obj", { -15.2f, -6.3f, -30.9f }, { 10.f, 1.f, 4.f }, white, "white", areaKey);

	game.CreateBlock("floor14", floorModel, "TriFloor.obj", { 9.8f, -6.3f, 6.f }, { 5.f, 1.f, 9.f }, white, "white", areaKey);

	game.CreateBlock("floor15", floorModel, "TriFloor.obj", { -0.2f, -6.3f, 10.f }, { 5.f, 1.f, 5.f }, white, "white", areaKey);
	game.CreateBlock("floor16", floorModel, "TriFloor.obj", { 9.8f, -6.3f, -22.2f }, { 5.f, 1.f, 5.f }, white, "white", areaKey);
	game.CreateBlock("floor17", floorModel, "TriFloor.obj", { 40.1f, -6.3f, -22.2f }, { 5.f, 1.f, 5.f }, white, "white", areaKey);
	game.CreateBlock("floor18", floorModel, "TriFloor.obj", { 40.1f, -6.3f, 8.f }, { 5.f, 1.f, 5.f }, white, "white", areaKey);
	game.CreateBlock("floor19", floorModel, "TriFloor.obj", { 25.f, -6.3f, -7.f }, { 3.f, 1.f, 5.f }, white, "white", areaKey);
	game.CreateBlock("floor20", floorModel, "TriFloor.obj", { 30.f, -6.3f, 28.1f }, { 5.f, 1.f, 15.f }, white, "white", areaKey);
	game.CreateBlock("floor21", floorModel, "TriFloor.obj", { 45.f, -6.3f, 38.1f }, { 10.f, 1.f, 5.f }, white, "white", areaKey);


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