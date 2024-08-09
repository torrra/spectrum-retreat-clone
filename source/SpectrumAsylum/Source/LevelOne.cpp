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
	colliders.AddCollider<BoxBV>("ground level", std::string("starting area"), LibMath::Vector3{ 25.f, 1.5f, 25.f }, LibMath::Vector3{ 80.f, 1.5f, 80.f });

	// Area two on ground level, negative x and positive z
	colliders.AddCollider<BoxBV>("ground level", std::string("area2"), LibMath::Vector3{ -25.f, -0.3f, 25.f }, LibMath::Vector3{ 50.f, 10.f, 60.f });

	// Area three on ground level, negative x and z
	colliders.AddCollider<BoxBV>("ground level", std::string("area3"), LibMath::Vector3{ -25.f, -0.3f, -25.f }, LibMath::Vector3{ 35.f, 10.f, 35.f });

	// Area four on ground level, positve x and negative z
	colliders.AddCollider<BoxBV>("ground level", std::string("area4"), LibMath::Vector3{ 25.f, -0.3f, -25.f }, LibMath::Vector3{ 35.f, 10.f, 35.f });

	// Area one on second level (top of the stairs), same rules as ground level starting area
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
	Material*		darkGray = assets.Create<Material>("dark gray", LibMath::Vector4{DARK_GRAY, 1.0f}, 89.2f);
	Material*		white = assets.Create<Material>("white", LibMath::Vector4{ WHITE, 1.0f }, 89.2f);

	assets.Create<Material>("light gray", LibMath::Vector4{LIGHT_GRAY , 1.f }, 750.f);


	// Create ceiling meshes
	game.CreateFloor("ceil1", floorModel, { -10.2f, 5.9f, -13.9f }, { 60.f, 1.f, 30.f }, white, "world");
	game.CreateFloor("ceil2", floorModel, { 38.7f, 5.9f, 35.f }, { 15.f, 1.f, 20.f }, white, "areasec1");
	game.CreateFloor("ceil3", floorModel, { -16.f, 5.9f, 50.f }, { 40.f, 1.f, 20.f }, white, "areasec2");
	game.CreateFloor("ceil4", floorModel, { -25.f, 5.9f, 23.f }, { 29.7f, 1.f, 7.f }, white, "areasec2");
	game.CreateFloor("ceil5", floorModel, { 16.f, 11.9f, 16.f }, { 15.f, 1.f, 15.f }, white, "world");
	game.CreateFloor("ceil6", floorModel, { 8.f, 5.9f, 74.f }, { 35.f, 1.f, 19.f }, white, "areasec1");
	game.CreateFloor("ceil7", floorModel, { 15.f, 5.9f, -43.f }, { 35.f, 1.f, 5.f }, white, "areasec4");


	// Create stairs meshes
	game.CreateFloor("stair1", floorModel, { 19.4f, 5.f, 17.f }, { 5.f, 1.f, 1.f }, white, "world");
	game.CreateFloor("stair2", floorModel, { 19.4f, 4.f, 20.f }, { 5.f, 1.f, 1.f }, white, "world");
	game.CreateFloor("stair3", floorModel, { 19.4f, 3.f, 23.f }, { 5.f, 1.f, 1.f }, white, "world");
	game.CreateFloor("stair4", floorModel, { 19.4f, 2.f, 26.f }, { 5.f, 1.f, 1.f }, white, "world");
	game.CreateFloor("stair5", floorModel, { 19.4f, 1.f, 29.f }, { 5.f, 1.f, 1.f }, white, "world");


	// Place walls and floors
	InitStartingAreaWalls(game);
	InitAreaTwoWalls(game);
	InitAreaThreeWalls(game);
	InitAreaFourWalls(game);
	InitUnderground(game);

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
	Material*		lightGray = assets.Get<Material>("light gray");

	const char*		areaKey = "starting area";

	// Wall scales
	const LibMath::Vector3		scale{ 1.f,  3.f, 5.f };
	const LibMath::Vector3		scaleR{ 5.f,  3.f, 1.f };

	// Create walls with holes
	game.CreateWallWithHole("wall10", wallModel2Hole, { 35.f,  2.7f, 20.0f }, scale, lightGray, areaKey);
	game.CreateWallWithHole("wall10a", wallModelHole, { 30.f,  2.7f, 15.0f }, scaleR, lightGray, areaKey);
	game.CreateWallWithHole("wall10b", wallModelHole, { -10.f,  2.7f, -10.0f }, { 5.f,  1.f,  1.0f }, lightGray, areaKey);
	game.CreateWallWithHole("wall17", wallModelHole, { 5.f,  2.7f, 5.f }, scaleR, lightGray, areaKey);

	// Place 'a few' more walls
	game.CreateWall("wall5", wallModel, { 45.f,  2.7f, 40.f }, scaleR, lightGray, areaKey);
	game.CreateWall("wall5b", wallModel, { 37.f,  2.7f, 40.f }, { 3.f, 3.f,  1.f }, lightGray, areaKey);
	game.CreateWall("wall6",  wallModel2, { 24.8f, 2.7f, 45.f },   scale, lightGray, areaKey);
	game.CreateWall("wall6b", wallModel2, { 24.8f, 2.7f, 35.f }, scale, lightGray, areaKey);
	game.CreateWall("wall6c", wallModel2, { 24.8f, 2.7f, 25.f }, scale, lightGray, areaKey);
	game.CreateWall("wall6d", wallModel2, { 24.8f, 2.7f, 15.f }, scale, lightGray, areaKey);
	game.CreateWall("wall8", wallModel2, { 35.f,  2.7f, 35.1f }, { 1.f,  3.f,  5.f }, lightGray, areaKey);

	game.CreateWall("wall2", wallModel, { 45.f, 2.7f, 50.f }, scaleR, lightGray, areaKey);
	game.CreateWall("wall2a", wallModel, { 35.f, 2.7f, 50.f }, scaleR, lightGray, areaKey);
	game.CreateWall("wall2b", wallModel,  { 25.f, 2.7f, 50.f }, scaleR, lightGray, areaKey);
	game.CreateWall("wall2c", wallModel, { 15.f, 2.7f, 50.f }, scaleR, lightGray, areaKey);

	game.CreateWall("wall4", wallModel2, { 50.f, 2.7f, 45.f }, scale, lightGray, areaKey);
	game.CreateWall("wall4b", wallModel2, { 50.f, 2.7f, 35.f }, scale, lightGray, areaKey);
	game.CreateWall("wall4c", wallModel2, { 50.f, 2.7f, 25.f }, scale, lightGray, areaKey);
	game.CreateWall("wall4d", wallModel2, { 50.f, 2.7f, 15.f }, scale, lightGray, areaKey);
	game.CreateWall("wall4e", wallModel2, { 50.f, 2.7f,  5.f }, scale, lightGray, areaKey);
	game.CreateWall("wall4f", wallModel2, { 50.f, 2.7f,  5.f }, scale, lightGray, areaKey);

	game.CreateWall("wall12", wallModel2, { 35.f,  2.7f, 10.f }, scale, lightGray, areaKey);
	game.CreateWall("wall12b", wallModel2, { 35.f,  2.7f, 0.f }, scale, lightGray, areaKey);

	game.CreateWall("wall14", wallModel2, { 24.8f, 2.7f, 5.f }, scale, lightGray, areaKey);

	game.CreateWall("wall16", wallModel, { 20.f,  2.7f, 15.f }, scaleR, lightGray, areaKey);
	game.CreateWall("wall16b", wallModel, { 10.f,  2.7f, 15.f }, scaleR, lightGray, areaKey);
	game.CreateWall("wall16c", wallModel, { 0.f,  2.7f, 15.f }, scaleR, lightGray, areaKey);

	game.CreateWall("wall28", wallModel2, { 10.1f, 2.7f, 4.1f }, { 1.f,  3.f,  1.f }, lightGray, areaKey);

	game.CreateWall("wall45", wallModel, { 20.f, 2.7f, 39.5f }, { 5.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall46", wallModel, { 7.4f, 2.7f, 50.f }, { 2.6f, 3.f, 1.f }, lightGray, areaKey);

	game.CreateWall("wall67", wallModel2, { 42.5f, 2.7f, 67.f }, { 1.f, 3.f, 15.f }, lightGray, areaKey);
	game.CreateWall("wall68", wallModel, { 4.f, 2.7f, 50.f }, { 10.f, 3.f, 1.f }, lightGray, areaKey);

	game.CreateWall("wall70", wallModel, { 10.f, 2.7f, 82.f }, { 35.f, 3.f, 1.f }, lightGray, areaKey);

	game.CreateWall("wall72", wallModel, { 8.5f, 2.7f, 72.f }, { 24.5f, 3.f, 1.f }, lightGray, "world");


	// Place floors
	game.CreateFloor("floor1", floorModel, { 37.5f, -0.3f, 45.f }, { 12.7f, 1.f, 4.9f }, white, areaKey);
	game.CreateFloor("floor2", floorModel, { 29.9f, -0.3f, 27.6f }, { 5.1f, 1.f, 12.5f }, white, areaKey);
	game.CreateFloor("floor3", floorModel, { 42.4f, -0.3f, 17.f }, { 7.5f, 1.f, 22.9f }, white, areaKey);
	game.CreateFloor("floor5", floorModel, { 29.9f, -0.3f, 4.5f }, { 5.f, 1.f, 10.4f }, white, areaKey);

	game.CreateFloor("floor22", floorModel, { 14.8f, -0.3f, 45.f }, { 10.f, 1.f, 5.f }, white, areaKey);
	game.CreateFloor("floor23", floorModel, { 4.6f, -0.3f, 35.f }, { 20.f, 1.f, 5.f }, white, "world");
	game.CreateFloor("floor24", floorModel, { 19.8f, -0.3f, 22.6f }, { 5.f, 1.f, 7.4f }, white, areaKey);

	game.CreateFloor("floor33", floorModel, { 3.4f, -0.3f, 55.f }, { 3.f, 1.f, 5.f }, white, areaKey);

	game.CreateFloor("ceil8", floorModel, { 52.f, -0.3f, 38.f }, { 20.f, 1.f, 5.f }, white, areaKey);

	// Place upper level walls
	areaKey = "second level";
	game.CreateWall("wall99", wallModel, { 14.f, 8.9f, 30.5f }, { 10.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall100", wallModel2, { 4.5f, 8.9f, 17.f }, { 1.f, 3.f, 15.f }, lightGray, areaKey);
	game.CreateWall("wall101", wallModel, { 14.f, 8.9f, 3.f }, { 10.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall102", wallModel2, { 24.f, 8.9f, 17.f }, { 1.f, 3.f, 15.f }, lightGray, areaKey);
	game.CreateWall("wall103", wallModel2, { 15.f, 8.7f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray, areaKey);
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
	Material*	lightGray = assets.Get<Material>("light gray");


	const char*		areaKey = "area2";

	// Wall scale
	const LibMath::Vector3 scaleR{ 5.f,  3.f, 1.f };

	// Place walls with holes
	game.CreateWallWithHole("wall17b", wallModelHole, { -5.f,  2.7f, 5.f }, scaleR, lightGray, areaKey);
	game.CreateWallWithHole("wall81", wallModel2Hole, { -25.5f,  2.7f, 13.4f }, { 1.f, 3.f,  2.f }, lightGray, areaKey);
	game.CreateWallWithHole("wall82", wallModelHole, { 20.f,  2.7f, -24.9f }, { 5.f, 3.f,  1.f }, lightGray, areaKey);
	game.CreateWallWithHole("wall74", wallModelHole, { -34.f, 2.7f, 5.f }, scaleR, lightGray, areaKey);


	// Place 'a couple' other walls
	game.CreateWall("wall16d", wallModel, { -10.f,  2.7f, 15.f }, scaleR, lightGray, areaKey);
	game.CreateWall("wall17c", wallModel, { -15.f,  2.7f, 5.f }, scaleR, lightGray, areaKey);

	game.CreateWall("wall23", wallModel2, { -15.2f, 2.7f,  4.1f }, { 1.f,  3.f,  1.f }, lightGray, areaKey);
	game.CreateWall("wall24", wallModel2, { -24.1f, 2.7f,  0.f }, { 1.f,  3.f, 5.1f }, lightGray, areaKey);
	game.CreateWall("wall25", wallModel2, { -15.2f, 2.7f,  12.5f }, { 1.f,  3.f, 7.5f }, lightGray, areaKey);


	game.CreateWall("wall47", wallModel, { -0.3f, 2.7f, 30.f }, { 15.2f, 3.f, 1.f }, lightGray, areaKey);

	game.CreateWall("wall48", wallModel, { -5.3f, 2.7f, 40.f }, { 10.1f, 3.f, 1.f }, lightGray, areaKey);

	game.CreateWall("wall50", wallModel2, { 14.8f, 2.7f, 22.7f }, { 1.f, 3.f, 7.2f }, lightGray);
	game.CreateWall("wall51", wallModel2, { 4.8f, 2.7f, 45.f }, { 1.f, 3.f, 5.1f }, lightGray, areaKey);
	game.CreateWall("wall52", wallModel2, { 4.5f, 2.7f, 23.f }, { 1.f, 3.f, 7.f }, lightGray, areaKey);

	game.CreateWall("wall53", wallModel2, { -15.6f, 2.7f, 55.f }, { 1.f, 3.f, 7.f }, lightGray, areaKey);

	game.CreateWall("wall54", wallModel2, { -5.4f, 2.7f, 45.f }, { 1.f, 3.f, 5.f }, lightGray, areaKey);

	game.CreateWall("wall55", wallModel2, { -25.4f, 2.7f, 6.4f }, { 1.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall56", wallModel2, { -25.4f, 2.7f, 23.f }, { 1.f, 3.f, 8.f }, lightGray, areaKey);
	game.CreateWall("wall57", wallModel2, { -25.4f, 2.7f, 39.f }, { 1.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall58", wallModel2, { -25.4f, 2.7f, 49.f }, { 1.f, 3.f, 3.f }, lightGray, areaKey);
	game.CreateWall("wall59", wallModel2, { -25.4f, 2.7f, 63.f }, { 1.f, 3.f, 5.f }, lightGray, areaKey);


	game.CreateWall("wall59a", wallModel2, { 14.5f, -3.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray);
	game.CreateWall("wall59b", wallModel2, { 4.5f, -3.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray);
	game.CreateWall("wall59c", wallModel2, { 14.5f, -9.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray);
	game.CreateWall("wall59d", wallModel2, { 4.5f, -9.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray);

	game.CreateWall("wall59e", wallModel2, { 14.5f, -15.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray);
	game.CreateWall("wall59f", wallModel2, { 4.5f, -15.3f, 23.f }, { 1.f, 3.f, 7.5f }, lightGray);
	game.CreateWall("wall59g", wallModel, { 9.5f, -3.3f, 30.f }, scaleR, lightGray);
	game.CreateWall("wall59i", wallModel, { 9.5f, -9.3f, 30.f }, scaleR, lightGray);
	game.CreateWall("wall59j", wallModel, { 9.5f, -9.3f, 16.f }, scaleR, lightGray);
	game.CreateWall("wall59k", wallModel, { 9.5f, -15.3f, 30.f }, scaleR, lightGray);
	game.CreateWall("wall59l", wallModel, { 9.5f, -15.3f, 16.f }, scaleR, lightGray);

	game.CreateWall("wall60", wallModel2, { -32.4f, 2.7f, 48.f }, { 1.f, 3.f, 10.f }, lightGray, areaKey);
	game.CreateWall("wall61", wallModel2, { -40.f, 2.7f, 44.f }, { 1.f, 3.f, 6.f }, lightGray, areaKey);
	game.CreateWall("wall62", wallModel2, { -47.f, 2.7f, 52.f }, { 1.f, 3.f, 6.f }, lightGray, areaKey);
	game.CreateWall("wall64", wallModel, { -40.f, 2.7f, 38.f }, { 15.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall65", wallModel, { -40.f, 2.7f, 68.f }, { 15.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall66", wallModel, { -40.f, 2.7f, 58.f }, { 7.f, 3.f, 1.f }, lightGray, areaKey);

	game.CreateWall("wall69", wallModel2, { -25.f, 2.7f, 75.f }, { 1.f, 3.f, 7.f }, lightGray, areaKey);

	game.CreateWall("wall73", wallModel, { -10.7f, 2.7f, 62.f }, { 5.3f, 3.f, 1.f }, lightGray, areaKey);

	game.CreateWall("wall75", wallModel, { -35.7f, 2.7f, 15.f }, { 10.f, 3.f, 1.f }, lightGray, areaKey);


	game.CreateWall("wall95", wallModel, { -30.5f, 2.7f, 28.f }, { 5.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall96", wallModel2, { -34.5f, 2.7f, 33.f }, { 1.f, 3.f, 5.f }, lightGray, areaKey);
	game.CreateWall("wall97", wallModel, { -10.f, 2.7f, 50.f }, { 5.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall98", wallModel2, { -15.6f, 2.7f, 44.4f }, { 1.f, 3.f, 5.f }, lightGray, areaKey);


	// Create floors
	game.CreateFloor("floor32", floorModel, { -8.4f, -0.3f, 67.f }, { 7.f, 1.f, 5.f }, white, areaKey);

	game.CreateFloor("floor34", floorModel, { 13.4f, -0.3f, 67.f }, { 5.f, 1.f, 5.f }, white, "world");
	game.CreateFloor("floor35", floorModel, { 25.4f, -0.3f, 55.f }, { 5.f, 1.f, 5.f }, white, "world");
	game.CreateFloor("floor36", floorModel, { 37.4f, -0.3f, 67.f }, { 5.f, 1.f, 5.f }, white, "world");
	game.CreateFloor("floor37", floorModel, { 9.f, -0.3f, 77.f }, { 34.f, 1.f, 5.f }, white, "world");
	game.CreateFloor("floor38", floorModel, { -20.4f, -0.3f, 67.f }, { 5.f, 1.f, 6.f }, white, areaKey);
	game.CreateFloor("floor39", floorModel, { 9.7f, -18.3f, 23.1f }, { 5.f, 1.f, 7.f }, white, "world");
	game.CreateFloor("floor40", floorModel, { -35.4f, -0.3f, 10.f }, { 10.f, 1.f, 5.f }, white, areaKey);

	game.CreateFloor("floor8", floorModel, { -8.5f, -0.3f, 10.f }, { 6.6f, 1.f, 5.f }, white, areaKey);
	game.CreateFloor("floor25", floorModel, { -20.4f, -0.3f, 35.f }, { 5.f, 1.f, 30.f }, white, areaKey);
	game.CreateFloor("floor26", floorModel, { -10.4f, -0.3f, 45.f }, { 5.f, 1.f, 5.f }, white, areaKey);
	game.CreateFloor("floor27", floorModel, { -30.4f, -0.3f, 33.f }, { 5.f, 1.f, 5.f }, white, areaKey);
	game.CreateFloor("floor28", floorModel, { -45.4f, -0.3f, 53.f }, { 20.f, 1.f, 15.f }, white, areaKey);
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
	Material*		lightGray = assets.Get<Material>("light gray");

	const char*		areaKey = "area3";

	// Wall scale
	const LibMath::Vector3		scaleR{ 5.f,  3.f, 1.f };

	// Create walls
	game.CreateWall("wall18c", wallModel, { -10.f,  2.7f,-5.f }, scaleR, lightGray, areaKey);
	game.CreateWall("wall18d", wallModel, { -20.f,  2.7f,-5.f }, scaleR, lightGray, areaKey);

	game.CreateWall("wall20", wallModel, { -1.f,  2.7f,-24.9f }, { 1.f,  3.f,  1.f }, lightGray, areaKey);

	game.CreateWall("wall22", wallModel2, { -15.2f, 2.7f,-4.1f }, { 1.f,  3.f,  1.f }, lightGray, areaKey);

	game.CreateWall("wall76", wallModel, { -35.7f, 2.7f, -15.f }, { 10.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall77", wallModel2, { -25.5f, 2.7f, -5.f }, { 1.f, 3.f, 10.f }, lightGray, areaKey);


	// Place floor pieces
	game.CreateFloor("floor29", floorModel, { -20.f, -0.3f, -31.f }, { 5.f, 1.f, 4.f }, white, areaKey);
	game.CreateFloor("floor30", floorModel, { -6.f, -0.3f, -20.f }, { 9.f, 1.f, 19.1f }, white, areaKey);
	game.CreateFloor("floor41", floorModel, { -35.4f, -0.3f, -5.f }, { 10.f, 1.f, 10.f }, white, areaKey);
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
	Material*		lightGray = assets.Get<Material>("light gray");

	const char*		areaKey = "area4";

	// Wall scale
	const LibMath::Vector3	scale{ 1.f,  3.f, 5.f };


	// Place walls
	game.CreateWall("wall1", wallModel, { 0.f, 2.7f,-45.f }, { 50.f, 3.f, 1.f }, lightGray, areaKey);

	game.CreateWall("wall3", wallModel2, { -55.f, 2.7f,  10.f }, { 1.f,  3.f, 60.f }, lightGray, "world");

	game.CreateWall("wall4g", wallModel2, { 50.f, 2.7f, -5.f }, scale, lightGray, areaKey);
	game.CreateWall("wall4h", wallModel2, { 50.f, 2.7f,-15.f }, scale, lightGray, areaKey);


	game.CreateWall("wall9", wallModel, { 36.9f, 2.7f, -5.2f }, { 2.f,  3.f,  1.f }, lightGray, areaKey);

	game.CreateWall("wall11", wallModel, { 47.9f, 2.7f, -5.2f }, { 2.f,  3.f,  1.f }, lightGray, areaKey);

	game.CreateWall("wall13", wallModel, { 45.f,  2.7f,-15.f }, { 5.f,3.f,  1.f }, lightGray, areaKey);
	game.CreateWall("wall13b", wallModel, { 35.f,  2.7f,-15.f }, { 5.f,3.f,  1.f }, lightGray, areaKey);
	game.CreateWall("wall13c", wallModel, { 28.f,  2.7f,-15.f }, { 2.5f,3.f,  1.f }, lightGray, areaKey);

	game.CreateWall("wall15", wallModel2, { 24.8f, 2.7f,-10.f }, { 1.f,  3.f,  5.f }, lightGray, areaKey);


	game.CreateWall("wall17d", wallModel, { -25.f,  2.7f, 5.f }, { 5.f, 3.f,  1.f }, lightGray, "world");
	game.CreateWall("wall18", wallModel, { 10.f,  2.7f,-5.f }, { 5.f, 3.f,  1.f }, lightGray, areaKey);
	game.CreateWall("wall18b", wallModel, { 0.f,  2.7f,-5.f }, { 5.f, 3.f,  1.f }, lightGray, areaKey);

	game.CreateWall("wall19", wallModel, { 10.f,  2.7f,-24.9f }, { 5.f, 3.f,  1.f }, lightGray, areaKey);

	game.CreateWall("wall21", wallModel, { 11.f,  2.7f,-45.f }, { 14.f, 3.f,  1.f }, lightGray, areaKey);

	game.CreateWall("wall26", wallModel2, { 24.8f, 2.7f,-20.f }, scale, lightGray, areaKey);
	game.CreateWall("wall26b", wallModel2, { 24.8f, 2.7f,-30.f }, scale, lightGray, areaKey);
	game.CreateWall("wall26c", wallModel2, { 24.8f, 2.7f,-40.f }, scale, lightGray, areaKey);
	game.CreateWall("wall27", wallModel2, { 0.f,  2.7f,-10.f }, scale, lightGray, areaKey);
	game.CreateWall("wall27b", wallModel2, { 0.f,  2.7f,-20.f }, scale, lightGray, areaKey);
	game.CreateWall("wall27c", wallModel2, { 0.f,  2.7f,-30.f }, scale, lightGray, areaKey);
	game.CreateWall("wall27d", wallModel2, { 0.f,  2.7f,-40.f }, scale, lightGray, areaKey);

	game.CreateWall("wall29", wallModel2, { 10.1f, 2.7f,-4.1f }, { 1.f,  3.f,  1.f }, lightGray, areaKey);
	game.CreateWall("wall30", wallModel2, { 0.f,  2.7f, 4.1f }, { 1.f,  3.f,  1.f }, lightGray, areaKey);
	game.CreateWall("wall31", wallModel2, { 0.f,  2.7f,-4.1f }, { 1.f,  3.f,  1.f }, lightGray, areaKey);
	game.CreateWall("wall78", wallModel2, { -45.f, 2.7f, 0.f }, { 1.f, 3.f, 15.f }, lightGray, "world");


	// PLace floors
	game.CreateFloor("floor4", floorModel, { 37.5f, -0.3f, -10.4f }, { 12.6f, 1.f, 4.5f }, white, areaKey);
	game.CreateFloor("floor6", floorModel, { 11.5f, -0.3f, -4.9f }, { 13.4f, 1.f, 19.9f }, white, "world");
	game.CreateFloor("floor7", floorModel, { 11.4f, -0.3f, -34.9f }, { 13.3f, 1.f, 10.1f }, white, areaKey);
	game.CreateFloor("floor9", floorModel, { -8.6f, -0.3f, 0.f }, { 6.7f, 1.f, 5.f }, white, "world");
	game.CreateFloor("floor10", floorModel, { -19.6f, -6.3f, 0.f }, { 4.4f, 1.f, 5.f }, white, "world");

	game.CreateFloor("floor31", floorModel, { 45.f, -0.3f, -30.f }, { 21.f, 1.f, 20.f }, white, areaKey);

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
	game.CreateDoor("door1", { 35.f, 2.7f, 27.55f }, { 0.1f, 3.f, 2.5f }, redColor, doorModelR, "starting area");
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
	game.CreateDoor("door18", { 15.f, 2.7f, 45.f }, { 0.1f, 3.f, 5.1f }, orangeColor, doorModel, "starting area");

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
	Material*		lightGray = assets.Get<Material>("light gray");

	const char*		areaKey = "underground";

	// Place walls
	game.CreateWall("wall32", wallModel, { -19.7f, -3.5f, -5.f }, { 4.5f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall33", wallModel2, { -24.1f, -3.5f, 0.f }, { 1.f, 3.f, 5.f }, lightGray, areaKey);
	game.CreateWall("wall34", wallModel, { -19.7f, -3.5f, 5.f }, { 4.5f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall35", wallModel2, { -15.3f, -3.5f, -16.f }, { 1.f, 3.f, 11.f }, lightGray, areaKey);
	game.CreateWall("wall36", wallModel2, { -15.3f, -3.5f, 9.f }, { 1.f, 3.f, 6.f }, lightGray, areaKey);
	game.CreateWall("wall37", wallModel2, { -5.1f, -3.5f, -15.f }, { 1.f, 3.f, 20.f }, lightGray, areaKey);
	game.CreateWall("wall38", wallModel, { -15.2f, -3.5f, -35.f }, { 10.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall39", wallModel, { -20.2f, -3.5f, -27.f }, { 5.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall40", wallModel, { -0.2f, -3.5f, 15.f }, { 15.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall42", wallModel2, { -25.1f, -3.5f, -31.f }, { 1.f, 3.f, 3.9f }, lightGray, areaKey);

	game.CreateWall("wall83", wallModel, { 0.f, -3.5f, 5.2f }, { 5.f, 3.f, 1.f }, lightGray, "world");

	game.CreateWall("wall86", wallModel, { 25.f, -3.5f, -12.8f }, { 3.3f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall87", wallModel, { 25.f, -3.5f, -1.5f }, { 3.3f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall88", wallModel2, { 28.5f, -3.5f, -7.f }, { 1.f, 3.f, 5.5f }, lightGray, areaKey);
	game.CreateWall("wall89", wallModel2, { 21.5f, -3.5f, -7.f }, { 1.f, 3.f, 5.5f }, lightGray, areaKey);


	game.CreateWall("wall104", wallModel, { 23.f, -3.5f, -35.f }, { 30.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall105", wallModel2, { 50.f, -3.5f, -2.f }, { 1.f, 3.f, 35.f }, lightGray, areaKey);

	game.CreateWall("wall106", wallModel, { 15.f, -3.5f, 6.f }, { 3.f, 3.f, 1.f }, lightGray, areaKey);

	game.CreateWall("wall90", wallModel, { 45.f, -3.5f, 33.f }, { 10.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall91", wallModel, { 40.f, -3.5f, 43.f }, { 15.f, 3.f, 1.f }, lightGray, areaKey);
	game.CreateWall("wall92", wallModel2, { 35.f, -3.5f, 23.f }, { 1.f, 3.f, 10.5f }, lightGray, areaKey);
	game.CreateWall("wall93", wallModel2, { 25.5f, -3.5f, 28.5f }, { 1.f, 3.f, 16.f }, lightGray, areaKey);
	game.CreateWall("wall94", wallModel2, { 54.f, -3.5f, 38.f }, { 1.f, 3.f, 5.5f }, lightGray, areaKey);

	// Place wall with a hole
	game.CreateWallWithHole("wall85", wallModel2Hole, { 12.5f, -3.5f, 10.f }, { 1.f, 3.f, 5.f }, lightGray, areaKey);

	// Place floor tiles
	game.CreateFloor("floor11", floorModel, { -10.2f, -6.3f, -7.f }, { 5.f, 1.f, 22.f }, white, areaKey);
	game.CreateFloor("floor12", floorModel, { -15.2f, -6.3f, -30.9f }, { 10.f, 1.f, 4.f }, white, areaKey);

	game.CreateFloor("floor14", floorModel, { 9.8f, -6.3f, 6.f }, { 5.f, 1.f, 9.f }, white, areaKey);

	game.CreateFloor("floor15", floorModel, { -0.2f, -6.3f, 10.f }, { 5.f, 1.f, 5.f }, white, areaKey);
	game.CreateFloor("floor16", floorModel, { 9.8f, -6.3f, -22.2f }, { 5.f, 1.f, 5.f }, white, areaKey);
	game.CreateFloor("floor17", floorModel, { 40.1f, -6.3f, -22.2f }, { 5.f, 1.f, 5.f }, white, areaKey);
	game.CreateFloor("floor18", floorModel, { 40.1f, -6.3f, 8.f }, { 5.f, 1.f, 5.f }, white, areaKey);
	game.CreateFloor("floor19", floorModel, { 25.f, -6.3f, -7.f }, { 3.f, 1.f, 5.f }, white, areaKey);
	game.CreateFloor("floor20", floorModel, { 30.f, -6.3f, 28.1f }, { 5.f, 1.f, 15.f }, white, areaKey);
	game.CreateFloor("floor21", floorModel, { 45.f, -6.3f, 38.1f }, { 10.f, 1.f, 5.f }, white, areaKey);


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