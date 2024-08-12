// IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

// Game header
#include "Game.h"
#include "LibMath/Vector.h"
#include "LevelOne.h"

bool Game::m_doorHit = false;
bool Game::m_gameOver = false;

/*************\
* Constructor *
\*************/

// Constructor without parameter
Game::Game(void)
{
	InitSound(*this);
	m_menu = Menu(*this);
}

/************\
* Destructor *
\************/

// Destructor to destroy sound engine
Game::~Game(void)
{

	if (m_soundEngine)
		m_soundEngine->drop();
}

/********\
* Create *
\********/
// Create mesh wall
Mesh* Game::CreateBlock(const std::string& key, Model* model, const char* modelString, const LibMath::Vector3& pos, const LibMath::Vector3& scale, Material* mat, const char* materialKey, const char* areaKey)
{
	m_sceneFile << "[block]\nkey: " << key << " model: " << modelString << " position: " << pos << " scale: " <<
	scale << " material: " << materialKey << " area: " << areaKey << "\n\n";

	// Add wall mesh to the current level
	Mesh*	block = m_currentLevel.m_scene.AddChild<Mesh>(key, model);

	// Link the wall to the scene node
	block->LinkToNode(m_currentLevel.m_scene.GetNode(key));

	// Set the position of the wall
	block->Translate(pos);
	// Set the size of the wall
	block->Scale(scale);
	// Set the material of th wall
	block->m_material = mat;

	// Add a collider for the wall
	m_currentLevel.m_colliders.AddCollider<BoxBV>(areaKey, key);

	// Link to the scene node wall colliders
	m_currentLevel.m_colliders.LinkToNode(m_currentLevel.m_scene, key, key);

	return block;
}

// Create mesh wall with hole
Mesh* Game::CreateWallWithHole(const std::string& key, Model* model, const char* modelString, const LibMath::Vector3& pos, const LibMath::Vector3& scale, Material* mat, const char* materialKey, const char* areaKey)
{
	m_sceneFile << "[hole]\nkey: " << key << " model: " << modelString << " position: " << pos <<
	" materialKey: " << materialKey << " scale: " << scale << " area: " << areaKey << " hole_collider: ";

	// Add wall mesh to the current level
	Mesh*	wall = m_currentLevel.m_scene.AddChild<Mesh>(key, model);

	// Link the wall to the scene node
	wall->LinkToNode(m_currentLevel.m_scene.GetNode(key));

	// Set the position of the wall
	wall->Translate(pos);
	// Set the size of the wall
	wall->Scale(scale);
	// Set the material of th wall
	wall->m_material = mat;

	// Create a specific collider for the wall with hole
	PhysicsLib::HoledCollider* collider = m_currentLevel.m_colliders.AddCollider<PhysicsLib::HoledCollider>(areaKey, key, pos, scale, PhysicsLib::HOLED);

	// Link to the scene node wall colliders
	m_currentLevel.m_colliders.LinkToNode(m_currentLevel.m_scene, key, key);
	// Add to the key the "hole" to complete the key code
	std::string	holeKey = key + "_hole";

	m_sceneFile << holeKey << "\n\n";

	// Add a collider for the wall
	collider->m_hole = m_currentLevel.m_colliders.AddCollider<BoxBV>(areaKey, holeKey, pos, LibMath::Vector3{1.f, 1.f, 1.f}, PhysicsLib::IGNORE);

	return wall;
}

// Create floor mesh
Mesh* Game::CreateFloor(const std::string& key, Model* model, const char* modelString, const LibMath::Vector3& pos, const LibMath::Vector3& scale, Material* mat, const char* areaKey)
{
	m_sceneFile << "[block]\nkey: " << key << " model: " << modelString << " position: " << pos << " scale: " <<
	scale << " area: " << areaKey << "\n\n";

	// Add wall mesh to the current level
	Mesh*	block = m_currentLevel.m_scene.AddChild<Mesh>(key, model);

	// Link the wall to the scene node
	block->LinkToNode(m_currentLevel.m_scene.GetNode(key));

	// Set the position of the wall
	block->Translate(pos);
	// Set the size of the wall
	block->Scale(scale);
	// Set the material of th wall
	block->m_material = mat;

	// Add a collider for the wall
	m_currentLevel.m_colliders.AddCollider<BoxBV>(areaKey, key);

	// Link to the scene node wall colliders
	m_currentLevel.m_colliders.LinkToNode(m_currentLevel.m_scene, key, key);

	return block;
}

// Create point light
PointLight* Game::CreateLevelLight(const std::string& key, const LibMath::Vector3& pos, Material* meshMat, Material* edgeMat)
{
	// Create ResourceManager to access to the assets
	ResourceManager&	assets = m_currentLevel.m_assets;
	// Create a graph to access to the scene
	Graph<SceneNode>&	gameObjects = m_currentLevel.m_scene;

	// Create the model of the light
	Model*	lightModel = assets.Get<Model>("Light.obj");
	// Create the model of the edge of the light
	Model*	edgeModel = assets.Get<Model>("LightEdge.obj");

	// Create the key word for the light
	std::string	meshKey = key + " mesh";
	// Create the key word for the edge
	std::string	edgeKey = key + " edge";

	// Add the mesh light to the graph
	Mesh*	lightMesh = gameObjects.AddChild<Mesh>(meshKey, lightModel);
	// Add the mesh edge to the graph
	Mesh*	edgeMesh = gameObjects.AddChild<Mesh>(meshKey, edgeKey, edgeModel);

	// Add the light to the graph
	PointLight*	light = gameObjects.AddChild<PointLight>(meshKey, key);

	// Link to the scene node the light mesh
	lightMesh->LinkToNode(gameObjects.GetNode(meshKey));
	// Link to the scene node the edge mesh
	edgeMesh->LinkToNode(gameObjects.GetNode(edgeKey));

	// Assign a material to the light
	lightMesh->m_material = meshMat;
	// Assign a material to the edge of the light
	edgeMesh->m_material = edgeMat;

	// Set light mesh position
	lightMesh->Translate(pos);
	// Set light position
	light->SetPosition(pos);

	// Set ambient component of the light
	light->SetAmbient({ WHITE, 1.f });
	// Set diffuse component of the light
	light->SetDiffuse({ WHITE, 1.f });
	// Set specular component of the light
	light->SetSpecular({ WHITE, 1.f });
	// Set attenuation of the light
	light->SetAttenuation({ 1.0f, 0.14f, 0.07f });

	return light;
}

// Create color block
ColorBlock* Game::CreateColorBlock(const std::string& key, const LibMath::Vector3& pos, const Color& color, Model* model, const char* areaKey)
{
	// Create a graph to access to the scene
	Graph<SceneNode>&	gameObjects = m_currentLevel.m_scene;
	// Create a collider to access to the collider
	BVHierarchy&	colliders = m_currentLevel.m_colliders;

	// Add a block to the graph
	ColorBlock*	block = gameObjects.AddChild<ColorBlock>(key, color);

	// Set the key word of the mesh cube
	std::string	meshKey = key + " mesh";
	// Set the key word of the light
	std::string	lightKey = key + " light";

	// Add the mesh to the graph
	block->m_mesh = gameObjects.AddChild<Mesh>(key, meshKey, model);

	// Link the mesh to the scene node
	block->m_mesh->LinkToNode(gameObjects.GetNode(meshKey));
	// Set the position of the cube
	block->m_mesh->Translate(pos);

	// Set the material of the cube
	block->m_mesh->m_material = &block->m_mat;

	// Add the light to the graph
	block->m_light = gameObjects.AddChild<PointLight>(meshKey, lightKey);

	// Initialise the light with a position and a color
	block->m_light->InitPointLight(pos, color);

	// Add a collider in BVHierarchy
	block->m_collider = colliders.AddCollider<ColBoxBV>(areaKey, key, pos, block->m_mesh->m_scale);

	// Link the collider to the scene node
	colliders.LinkToNode(gameObjects, key, meshKey);

	// Set a color to the collider
	block->m_collider->m_color = &block->m_color;

	return block;

}

// Create a cube holder
Mesh* Game::CreateCubeHolder(const std::string& key, Model* model, const LibMath::Vector3& pos, const char* areaKey)
{
	// Create a resource manager to access to the assets
	ResourceManager&	assets = m_currentLevel.m_assets;

	// Add a mesh to the scene
	Mesh*	cubeHolder = m_currentLevel.m_scene.AddChild<Mesh>(key, model);

	// Link to the scene node the cube holder
	cubeHolder->LinkToNode(m_currentLevel.m_scene.GetNode(key));

	// Set a position to the cube holder
	cubeHolder->Translate({pos.m_x, pos.m_y - 1.5f, pos.m_z});
	// Set a size to the cube holder
	cubeHolder->Scale({1.5f, 0.66f, 1.5f});
	// Set a material to the cube holder
	cubeHolder->m_material = assets.Get<Material>("dark");

	return cubeHolder;
}

// Create a Door
Door* Game::CreateDoor(const std::string& key, const LibMath::Vector3& pos, const LibMath::Vector3& scale, const Color& color, Model* model, const char* areaKey)
{
	// Create a graph to access to the scene
	Graph<SceneNode>& gameObjects = m_currentLevel.m_scene;
	// Create a BVHierarchy to access to the collider
	BVHierarchy&	colliders = m_currentLevel.m_colliders;

	// Create the key of the door
	std::string	meshKey = key + " mesh";

	// Add the door to the graph
	Door*	door = gameObjects.AddChild<Door>(key, color);

	// Add the mesh to the graph
	door->m_mesh = gameObjects.AddChild<Mesh>(key, meshKey, model);
	// Link the mesh to the scene
	door->m_mesh->LinkToNode(gameObjects.GetNode(meshKey));

	// Set the position of the door
	door->m_mesh->Translate(pos);
	// Set the size of the door
	door->m_mesh->Scale(scale);

	// Init the material of the door
	door->m_mesh->m_material = &door->m_mat;

	// Set the material of the door
	door->m_mesh->m_material->InitMaterial
	(
		{ door->m_doorColor.m_red, door->m_doorColor.m_green, door->m_doorColor.m_blue, 0.5f },
		{ door->m_doorColor.m_red, door->m_doorColor.m_green, door->m_doorColor.m_blue, 0.5f },
		{ door->m_doorColor.m_red, door->m_doorColor.m_green, door->m_doorColor.m_blue, 0.5f },
		{ door->m_doorColor.m_red * 0.2f, door->m_doorColor.m_green * 0.2f, door->m_doorColor.m_blue * 0.2f, 0.5f },
		32.f
	);

	// Add a collider to the door
	door->m_collider = colliders.AddCollider<BoxBV>(areaKey, key, door->m_mesh->m_position, door->m_mesh->m_scale, PhysicsLib::DOOR);
	// Link the collider to the scene
	colliders.LinkToNode(gameObjects, key, meshKey);

	// Set the position of the collider on the door position
	door->m_collider->m_position = door->m_mesh->m_position;
	// Set the size of the collider to the size of the door
	door->m_collider->m_boxScale = door->m_mesh->m_scale;
	// Calculate the min and the max vertex
	door->m_collider->m_minVertex = door->m_collider->m_position - door->m_collider->m_boxScale;
	door->m_collider->m_maxVertex = door->m_collider->m_position + door->m_collider->m_boxScale;

	return door;
}

// Create teleporter
PhysicsLib::Teleporter* Game::CreateTeleporters(const std::string& key, const LibMath::Vector3& pos, const LibMath::Vector3& otherPos, const char* areaKey)
{
	// Create a collider
	BVHierarchy&	colliders = m_currentLevel.m_colliders;

	// Add the teleporter collider
	PhysicsLib::Teleporter*	teleporter = colliders.AddCollider<PhysicsLib::Teleporter>(areaKey, std::string(key), pos, LibMath::Vector3(5.f, 5.f, 5.f), PhysicsLib::TELEPORTER);

	// Set the key of the teleporter
	std::string	otherKey = key + " other";

	// Add another collider
	teleporter->m_otherSide = colliders.AddCollider<PhysicsLib::Teleporter>(areaKey, otherKey, otherPos, LibMath::Vector3(5.f, 5.f, 5.f), PhysicsLib::TELEPORTER);

	teleporter->m_otherSide->m_otherSide = teleporter;

	return teleporter;
}

// Reset the level
void Game::ResetLevel(void)
{
	// Destroy the current level
	m_currentLevel.~Level();

	// Create a new level
	new (&m_currentLevel) Level();
}

// Update the different screen
void Game::Update()
{
	// New IMGUI frame for game UI (title screen, pause menu, game over)
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Switch between different state
	switch (m_currentState)
	{
	case TITLE_SCREEN:
		m_window.SetWindowColor({BLACK, 1.0f}, RENDER_MODE::FILL);
		m_menu.UpdateStartMenu(*this);
		break;
	case PAUSE:
		m_window.SetWindowColor({BLACK, 1.0f}, RENDER_MODE::FILL);
		m_menu.UpdatePauseMenu(*this);
		break;
	case IN_GAME:
		UpdateLevelOne(*this);
		break;
	case GAME_OVER:
		// Condition met when level is complete
		m_window.SetWindowColor({BLACK, 1.0f}, RENDER_MODE::FILL);
		m_menu.UpdateEndMenu(*this);
		break;

	default:
		break;
	}

	// Update imgui
	ImGui::Render();

	if (m_currentState == IN_GAME && m_menu.GetBackgroundID())
	{
		m_menu.ResetID();
		return;
	}

	ImDrawData*	data;

	data = ImGui::GetDrawData();

	ImGui_ImplOpenGL3_RenderDrawData(data);

	// Update the screen
	m_window.UpdateWindow();
}

void Game::ReadSceneFile()
{
	std::string		  currentObject;

	m_sceneBuf << m_sceneFile.rdbuf();
	m_sceneBuf >> currentObject;

	while (currentObject.size())
	{
		//m_sceneFile >> currentObject;

		switch (currentObject[1])
		{
		case 'b':
			ReadBlock();
			break;
		case 'h':
			ReadHoledBlock();
			break;

		default: break;
		}

		currentObject = std::string();
		m_sceneBuf >> currentObject;
	}
}

void Game::ReadBlock()
{
	std::string			 value1, value2, type;
	std::stringstream    sstream;
	LibMath::Vector3	 vector;

	m_sceneBuf >> type >> value1 >> type >> value2;


	Model*		model = m_currentLevel.m_assets.Get<Model>(value2);

	// Add wall mesh to the current level
	Mesh* block = m_currentLevel.m_scene.AddChild<Mesh>(value1, model);

	// Link the wall to the scene node
	block->LinkToNode(m_currentLevel.m_scene.GetNode(value1));

	m_sceneBuf >> type >> value2;
	sstream = std::stringstream(value2);
	sstream >> vector;

	// Set the position of the wall
	block->Translate(vector);

	// Set the size of the wall
	m_sceneBuf >> type >> value2;
	sstream = std::stringstream(value2);
	sstream >> vector;

	block->Scale(vector);

	// Set the material of th wall
	m_sceneBuf >> type >> value2;

	Material* material = m_currentLevel.m_assets.Get<Material>(value2);

	block->m_material = material;

	// Add a collider for the wall
	m_sceneBuf >> type >> value2;

	m_currentLevel.m_colliders.AddCollider<BoxBV>(value2, value1);

	// Link to the scene node wall colliders
	m_currentLevel.m_colliders.LinkToNode(m_currentLevel.m_scene, value1, value1);

}

void Game::ReadHoledBlock()
{

	std::string			 value1, value2, type;
	std::stringstream    sstream;
	LibMath::Vector3	 position, scale;

	m_sceneBuf >> type >> value1 >> type >> value2;

	Model* model = m_currentLevel.m_assets.Get<Model>(value2);

	// Add wall mesh to the current level
	Mesh* wall = m_currentLevel.m_scene.AddChild<Mesh>(value1, model);

	// Link the wall to the scene node
	wall->LinkToNode(m_currentLevel.m_scene.GetNode(value1));

	m_sceneBuf >> type >> value2;
	sstream = std::stringstream(value2);
	sstream >> position;

	// Set the material of th wall
	m_sceneBuf >> type >> value2;

	Material* material = m_currentLevel.m_assets.Get<Material>(value2);

	m_sceneBuf >> type >> value2;
	sstream = std::stringstream(value2);
	sstream >> scale;

	// Set the position of the wall
	wall->Translate(position);
	// Set the size of the wall
	wall->Scale(scale);


	// Set the material of th wall
	wall->m_material = material;


	// Set the material of th wall
	m_sceneBuf >> type >> value2;

	// Create a specific collider for the wall with hole
	PhysicsLib::HoledCollider* collider =
	m_currentLevel.m_colliders.AddCollider<PhysicsLib::HoledCollider>
	(
		value2, value1, position, scale, PhysicsLib::HOLED
	);

	// Link to the scene node wall colliders
	m_currentLevel.m_colliders.LinkToNode(m_currentLevel.m_scene, value1, value1);
	// Add to the key the "hole" to complete the key code
	std::string	holeKey = value1 + "_hole";


	// Add a collider for the wall
	collider->m_hole = m_currentLevel.m_colliders.AddCollider<BoxBV>
	(
		value2, holeKey, position, LibMath::Vector3{ 1.f, 1.f, 1.f }, PhysicsLib::IGNORE
	);

}
