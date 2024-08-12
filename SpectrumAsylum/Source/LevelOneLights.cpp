#include "LevelOneLights.h"
#include "Light.h"
#include "LightVolume.h"
#include "ColorMacros.h"

void PlaceLights(Game& game)
{
	// References for readability
	ResourceManager&	assets = game.m_currentLevel.m_assets;
	BVHierarchy&		colliders = game.m_currentLevel.m_colliders;

	// Get white material from ressource manager
	Material*			white = assets.Get<Material>("white");

	const float			alpha = 1.f;

	// Create material for light edges
	Material*			edgeMat = assets.Create<Material>("edge material",
					LibMath::Vector4{ SUPER_DARK_GRAY, alpha },
					LibMath::Vector4{ DARKER_DARK_GRAY, alpha },
					LibMath::Vector4{ SLIGHTLY_LIGHT_GRAY, alpha },
					38.4f);

	// Lights next to player spawnpoint
	PointLight*		spawnPointLight1 = game.CreateLevelLight("spawnPointLight1",{ 45.f, 4.5f, 45.f }, white, edgeMat);
	PointLight*		spawnPointLight2 = game.CreateLevelLight("spawnPointLight2", { 43.f, 4.5f, 33.f }, white, edgeMat);
	PointLight*		spawnPointLight3 = game.CreateLevelLight("spawnPointLight3", { 43.f, 4.5f, 4.5f }, white, edgeMat);
	PointLight*		spawnPointLight4 = game.CreateLevelLight("light facing spawnpoint", { 15.6557f, 4.5f, 44.2081f }, white, edgeMat);

	// Lights before falling into hole
	PointLight*		corridorLight1 = game.CreateLevelLight("corridor1", { 30.f, 4.5f, 5.65916f }, white, edgeMat);
	PointLight*		rightToHole = game.CreateLevelLight("light right to hole", { 8.13134f, 4.5f, -15.7522f }, white, edgeMat);
	PointLight*		aboveHole = game.CreateLevelLight("light above hole", { -8.99016f, 4.45205f, -0.123407f }, white, edgeMat);

	// Underground lights
	PointLight*		underGroundLight = game.CreateLevelLight("underground light", { 25.1362f, -1.5f, -7.19665f }, white, edgeMat);
	PointLight*		undergroundCorridor = game.CreateLevelLight("underground corridor light", { 30.5f, -1.5f, 20.f }, white, edgeMat);
	PointLight*		light1 = game.CreateLevelLight("light1", { 10.f, -1.4f, -22.5f }, white, edgeMat);
	PointLight*		light2 = game.CreateLevelLight("light2", { 40.f, -1.4f, -22.f }, white, edgeMat);
	PointLight*		light3 = game.CreateLevelLight("light3", { 40.f, -1.4f, 8.f }, white, edgeMat);

	// Post-teleportation lights (back of the level)
	PointLight*		bottomStairs = game.CreateLevelLight("bottom stair light", { 23.0493f, 4.5f, 37.7347f }, white, edgeMat);
	PointLight*		light4 = game.CreateLevelLight("light4", { -20.f, 4.5f, 34.5f }, white, edgeMat);
	PointLight*		light5 = game.CreateLevelLight("light5", { -34.f, 4.5f, 10.f }, white, edgeMat);

	// light6 was removed

	// More lights at the back of the level
	PointLight*		light7 = game.CreateLevelLight("light7", { -29.f, 4.5f, 63.f }, white, edgeMat);
	PointLight*		light8 = game.CreateLevelLight("light8", { -49.f, 4.5f, 63.f }, white, edgeMat);
	PointLight*		light9 = game.CreateLevelLight("light9", { -47.5f, 4.5f, 42.f }, white, edgeMat);
	PointLight*		light10 = game.CreateLevelLight("light10", { -40.f, 4.5f, 55.f }, white, edgeMat);
	PointLight*		light11 = game.CreateLevelLight("light11", { -20.f, 4.5f, 72.f }, white, edgeMat);
	PointLight*		light12 = game.CreateLevelLight("light12", { 12.f, 4.5f, 77.f }, white, edgeMat);

	// Light at the top of the stairs
	PointLight*		light13 = game.CreateLevelLight("light13", { 15.f, 10.5f, 9.5f }, white, edgeMat);


	// Light volumes next to spawnpont
	LightBox*		lightBox1 = colliders.AddCollider<LightBox>("start", std::string("lightBox1"));
	LightBox*		lightBox2 = colliders.AddCollider<LightBox>("start", std::string("lightBox2"));

	// Light volumes in the back (next to hole)
	LightBox*		corridorBox1 = colliders.AddCollider<LightBox>("world", std::string("corridorBox1"));
	LightBox*		boxRightToHole = colliders.AddCollider<LightBox>("area4", std::string("box right to hole"));
	LightBox*		boxAboveHole = colliders.AddCollider<LightBox>("world", std::string("box above hole"));

	// Underground light volume
	LightBox*		undergroundBox = colliders.AddCollider<LightBox>("underground", std::string("level-1 box"));

	// Post-teleportation light volumes in the back
	LightBox*		backroomsBox = colliders.AddCollider<LightBox>("ground level", std::string("backrooms box"));
	LightBox*		bottomStairsBox = colliders.AddCollider<LightBox>("start", std::string("bottom stairs box"));
	LightBox*		backCorridor = colliders.AddCollider<LightBox>("ground level", std::string("back corridor box"));
	LightBox*		backBox = colliders.AddCollider<LightBox>("ground level", std::string("back level box"));

	// Top of the stairs
	LightBox* ceilingBox = colliders.AddCollider<LightBox>("second level", std::string("ceiling light box"));


	// Translate and scale light volumes
	lightBox1->SetColliderTransform({ 45.f, 1.f, 45.f }, { 40.f, 2.5f, 7.f });
	lightBox2->SetColliderTransform({ 45.f, 1.f, 5.f }, { 20.f, 2.5f, 35.f });

	corridorBox1->SetColliderTransform({ 35.f, 1.f, -15.65916f }, { 15.f, 2.5f, 30.f });
	boxRightToHole->SetColliderTransform({ 14.6712f ,1.4535f, -10.6112f }, { 15.f, 2.5f, 15.f });
	boxAboveHole->SetColliderTransform({ 0.f, 1.f, 0.f }, { 25.f, 2.5f, 5.f });

	undergroundBox->SetColliderTransform({ 25.1362f, -5.f, -7.19665f }, { 80.f, 2.5f, 80.f });

	backroomsBox->SetColliderTransform({ -36.2f, 1.3f, 40.f }, { 45.f, 1.5f, 70.f });
	backCorridor->SetColliderTransform({ 12.f, 1.5f, 77.f }, { 45.f, 1.5f, 15.f });
	bottomStairsBox->SetColliderTransform({ 5.02217f, 1.f, 34.8133f }, { 20.f, 1.5f, 15.f });
	backBox->SetColliderTransform({ -20.f, 1.5f, 35.f }, { 60.f, 1.5f, 85.f });

	ceilingBox->SetColliderTransform({ 15.f, 5.5f, 9.5f }, { 35.f, 3.5f, 35.f });


	// Add lights to be handled by each light volume

	lightBox1->AddLight(spawnPointLight1);
	lightBox1->AddLight(spawnPointLight4);

	lightBox2->AddLight(spawnPointLight2);
	lightBox2->AddLight(spawnPointLight3);

	corridorBox1->AddLight(corridorLight1);

	boxRightToHole->AddLight(rightToHole);

	boxAboveHole->AddLight(aboveHole);

	undergroundBox->AddLight(underGroundLight);
	undergroundBox->AddLight(undergroundCorridor);
	undergroundBox->AddLight(light1);
	undergroundBox->AddLight(light2);
	undergroundBox->AddLight(light3);

	bottomStairsBox->AddLight(bottomStairs);

	backroomsBox->AddLight(light4);
	backroomsBox->AddLight(light5);
	backroomsBox->AddLight(light7);
	backroomsBox->AddLight(light8);
	backroomsBox->AddLight(light9);
	backroomsBox->AddLight(light10);

	backCorridor->AddLight(light12);

	ceilingBox->AddLight(light13);

	backBox->AddLight(light11);


	// Create lights that will be handled by other light volumes (see PlaceColorCubes fucntion)
	game.CreateLevelLight("cube room light", { 11.7971f, 4.5f, -35.1758f }, white, edgeMat);
	game.CreateLevelLight("light14", { 13.f, 4.5f, 70.f }, white, edgeMat);
	game.CreateLevelLight("light15", { 37.f, 4.5f, 68.f }, white, edgeMat);

}

void PlaceColorCubes(Game& game)
{
	// References for readability
	ResourceManager& assets = game.m_currentLevel.m_assets;
	Graph<SceneNode>& gameObjects = game.m_currentLevel.m_scene;
	BVHierarchy& colliders = game.m_currentLevel.m_colliders;

	// Color objects for colored cubes
	Color		blueColor = { BLUE, 1.f };
	Color		orangeColor = { ORANGE, 1.f };
	Color		redColor = { RED, 1.f };
	Color		whiteColor = { WHITE, 1.f };
	Color		greenColor = { GREEN, 1.f };


	// Get cube model from ressource manager
	Model* cubeModel = assets.Get<Model>("cube.obj");

	// Create first cubes (next to spawn point)
	ColorBlock*		cubeSpawn = game.CreateColorBlock("cube", { 29.8f, 2.f, 20.f }, redColor, cubeModel);
	ColorBlock*		cube1 = game.CreateColorBlock("cube1", { 5.f, 2.f, 8.f }, whiteColor, cubeModel);
	ColorBlock*		cube2 = game.CreateColorBlock("cube2", { -5.f, 2.f, 8.f }, redColor, cubeModel);

	// Create cube holder meshes for those cubes
	game.CreateCubeHolder("cubeHolder1", cubeModel, cubeSpawn->m_mesh->m_position);
	game.CreateCubeHolder("cubeHolder2", cubeModel, cube1->m_mesh->m_position);
	game.CreateCubeHolder("cubeHolder3", cubeModel, cube2->m_mesh->m_position);


	// Cubes next to hole
	ColorBlock*		cube3 = game.CreateColorBlock("cube3", { 20.f, 2.f, -27.f }, redColor, cubeModel);
	ColorBlock*		cube4 = game.CreateColorBlock("cube4", { 20.f, 2.f, -35.f }, whiteColor, cubeModel);

	// Holders
	game.CreateCubeHolder("cubeHolder4", cubeModel, cube3->m_mesh->m_position);
	game.CreateCubeHolder("cubeHolder5", cubeModel, cube4->m_mesh->m_position);

	// Underground cubes
	ColorBlock*		cube5 = game.CreateColorBlock("cube5", { -10.f, -1.3f, 0.f }, orangeColor, cubeModel);
	ColorBlock*		cube6 = game.CreateColorBlock("cube6", { 15.f, -3.7f, 10.f }, blueColor, cubeModel);
	ColorBlock*		cube7 = game.CreateColorBlock("cube7", { 21.f, -3.5f, -7.f }, greenColor, cubeModel);

	ColorBlock*		cube8 = game.CreateColorBlock("cube8", { 29.f, -3.5f, -7.f }, whiteColor, cubeModel);

	// Transform underground cubes (for easier ray intersections)
	cube8->m_mesh->Scale({ 1.5f, 1.f, 1.f });


	// Final tower cubes
	ColorBlock*		cube9 = game.CreateColorBlock("cube9", { 5.f, -10.f, 23.f }, whiteColor, cubeModel);
	ColorBlock*		cube10 = game.CreateColorBlock("cube10", { 2.f, -4.f, 23.f }, whiteColor, cubeModel);
	ColorBlock*		cube11 = game.CreateColorBlock("cube11", { 5.f, 2.f, 23.f }, whiteColor, cubeModel);


	// Enable colored cube movement
	cube9->m_mesh->Scale({ 1.5f, 1.f, 1.f });
	cube10->m_mesh->Scale({ 1.5f, 1.f, 1.f });
	cube11->m_mesh->Scale({ 1.5f, 1.f, 1.f });

	// Enable cube movement along the x axis
	cube9->SetMovement(2.4f, 3.f, ColorBlock::LEFT, ColorBlock::RIGHT);
	cube10->SetMovement(2.4f, 3.f, ColorBlock::RIGHT, ColorBlock::LEFT);
	cube11->SetMovement(2.4f, 3.f, ColorBlock::LEFT, ColorBlock::RIGHT);

	// Holders
	game.CreateCubeHolder("cubeHolder7", cubeModel, cube6->m_mesh->m_position);


	// Post-teleportation white cubes
	ColorBlock*		cube12 = game.CreateColorBlock("cube12", { -30.f, 2.f, 29.f }, whiteColor, cubeModel);
	ColorBlock*		cube13 = game.CreateColorBlock("cube13", { -16.5f, 2.f, 14.f }, whiteColor, cubeModel);
	ColorBlock*		cube14 = game.CreateColorBlock("cube14", { -34.f, 2.f, 1.f }, whiteColor, cubeModel);

	// Holders
	game.CreateCubeHolder("cubeHolder10", cubeModel, cube12->m_mesh->m_position);
	game.CreateCubeHolder("cubeHolder11", cubeModel, cube13->m_mesh->m_position);
	game.CreateCubeHolder("cubeHolder12", cubeModel, cube14->m_mesh->m_position);


	// Cubes behind walls
	ColorBlock*		cube15 = game.CreateColorBlock("cube15", { -34.f, 2.f, -5.f }, redColor, cubeModel);
	ColorBlock*		cube16 = game.CreateColorBlock("cube16", { -36.2f, 2.f, 40.f }, greenColor, cubeModel);

	// Holders
	game.CreateCubeHolder("cubeHolder13", cubeModel, cube15->m_mesh->m_position);
	game.CreateCubeHolder("cubeHolder14", cubeModel, cube16->m_mesh->m_position);

	// Moving white cube at the end of platforming challenge
	ColorBlock*		cube24 = game.CreateColorBlock("cube24", { -11.5f, 2.f, 81.5f }, whiteColor, cubeModel);

	// Enlarge cube along the z axis
	cube24->m_mesh->Scale({ 1.f, 1.f, 1.5f });

	// Enable movement along the x axis
	cube24->SetMovement(1.2f, 4.f, ColorBlock::LEFT, ColorBlock::RIGHT);


	// Blue cube in platforming challenge
	ColorBlock*		cube17 = game.CreateColorBlock("cube17", { 4.f, 2.f, 54.f }, blueColor, cubeModel);

	// Holder
	game.CreateCubeHolder("cubeHolder15", cubeModel, cube17->m_mesh->m_position);


	// First cube light volume
	LightBox*		redCubeBox = colliders.AddCollider<LightBox>("world", std::string("red cube1 light box"));

	// Transform AABB and add lights to handle
	redCubeBox->SetColliderTransform({ 40.f, 1.f, 35.f }, { 20.f, 2.5f, 55.f });
	redCubeBox->AddLight(cubeSpawn->m_light);


	// Light volume for the two cubes left to the hole
	LightBox*		nextToTrap = colliders.AddCollider<LightBox>("world", std::string("next to trap"));

	// Transform AABB and add lights to handle
	nextToTrap->SetColliderTransform({ 3.40729f, 2.f, 11.78987f }, { 35.f, 0.1f, 20.f });
	nextToTrap->AddLight(cube1->m_light);
	nextToTrap->AddLight(cube2->m_light);

	// Light volume for room with two cubes right to hole
	LightBox*		twoCubesRoom = colliders.AddCollider<LightBox>("world", std::string("two blue cubes box"));

	// Transform AABB and add lights
	twoCubesRoom->SetColliderTransform({ 11.7526f, 1.f, -5.2917f }, {30.f, 2.f, 50.f });
	twoCubesRoom->AddLight(cube3->m_light);
	twoCubesRoom->AddLight(cube4->m_light);

	// Add light previously created in PlaceLights
	twoCubesRoom->AddLight(GetObject<PointLight>(gameObjects, "cube room light"));


	// Light volume for orange cube right after falling into hole
	LightBox*		cubeLight2 = colliders.AddCollider<LightBox>("world", std::string("cubeLight2"));

	// Transform AABB and add lights
	cubeLight2->SetColliderTransform({ -10.f, -2.f, 0.f }, { 15.f, 2.f, 15.f });
	cubeLight2->AddLight(cube5->m_light);


	// Light volum for underground blue cube
	LightBox*		cubeLight3 = colliders.AddCollider<LightBox>("world", std::string("cubeLight3"));

	// Transform AABB and add light
	cubeLight3->SetColliderTransform({ 15.f, -4.4f, 10.f }, { 35.f, 2.f, 35.f });
	cubeLight3->AddLight(cube6->m_light);

	// Light volume for underground green cube
	LightBox*		cubeLight4 = colliders.AddCollider<LightBox>("world", std::string("cubeLight4"));

	// Transform AABB and add light
	cubeLight4->SetColliderTransform({ 21.f, -4.2f, -7.f }, { 15.f, 2.f, 15.f });
	cubeLight4->AddLight(cube7->m_light);

	// Light volume for underground white cube
	LightBox*		cubeLight5 = colliders.AddCollider<LightBox>("world", std::string("cubeLight5"));

	// Transform AABB ans add light
	cubeLight5->SetColliderTransform({ 29.f, -4.2f, -7.f }, { 50.f, 2.f, 50.f });
	cubeLight5->AddLight(cube8->m_light);


	// Light volume containing all 3 final tower cubes
	LightBox*		towerBox = colliders.AddCollider<LightBox>("world", std::string("towerBox"));

	// Transform AABB
	towerBox->SetColliderTransform({ 5.f, 0.f, 25.f }, { 50.f, 50.f, 35.f });

	// Add all 3 final tower cubes
	towerBox->AddLight(cube9->m_light);
	towerBox->AddLight(cube10->m_light);
	towerBox->AddLight(cube11->m_light);


	// Light volume for white cubes after teleportation
	LightBox*		cubeLight9 = colliders.AddCollider<LightBox>("world", std::string("cubeLight9"));
	LightBox*		cubeLight10 = colliders.AddCollider<LightBox>("world", std::string("cubeLight10"));
	LightBox*		cubeLight11 = colliders.AddCollider<LightBox>("world", std::string("cubeLight11"));

	// Transform AABBs and add lights
	cubeLight9->SetColliderTransform({ -30.f, 1.3f, 29.f }, { 25.f, 2.f, 30.f });
	cubeLight9->AddLight(cube12->m_light);

	cubeLight10->SetColliderTransform({ -16.5f, 1.f, 14.f }, { 25.f, 2.f, 30.f });
	cubeLight10->AddLight(cube13->m_light);

	cubeLight11->SetColliderTransform({ -34.f, 1.3f, 1.f }, { 15.f, 2.f, 15.f });
	cubeLight11->AddLight(cube14->m_light);



	// Light volume for red cube behind walls
	LightBox*		cubeLight12 = colliders.AddCollider<LightBox>("world", std::string("cubeLight12"));

	// Transform AABB and add light
	cubeLight12->SetColliderTransform({ -34.f, 1.3f, -5.f }, { 15.f, 2.f, 15.f });
	cubeLight12->AddLight(cube15->m_light);

	// Light volume for green cube behind wall
	LightBox*		cubeLight13 = colliders.AddCollider<LightBox>("world", std::string("cubeLight13"));

	// Transform AABB and add light
	cubeLight13->SetColliderTransform({ -36.2f, 1.3f, 40.f }, { 7.f, 2.f, 25.f });
	cubeLight13->AddLight(cube16->m_light);


	// Light volume for blue cube at the end of platforming challenge
	LightBox*		cubeLight15 = colliders.AddCollider<LightBox>("world", std::string("cubeLight15"));

	// Transform AABB
	cubeLight15->SetColliderTransform({ 15.f, 1.3f, 70.f }, { 35.f, 2.f, 35.f });

	// Add blue cube light
	cubeLight15->AddLight(cube17->m_light);

	// Add lights in platforming challenge previously created in PlaceLights()
	cubeLight15->AddLight(GetObject<PointLight>(gameObjects, "light14"));
	cubeLight15->AddLight(GetObject<PointLight>(gameObjects, "light15"));

	// Light volume for moving white cube at the end of platforming challenge
	LightBox*		cubeLight16 = colliders.AddCollider<LightBox>("world", std::string("cubeLight16"));

	// Transform AABB and add light
	cubeLight16->SetColliderTransform({ -13.f, 1.3f, 82.5f }, { 60.f, 2.f, 50.f });
	cubeLight16->AddLight(cube24->m_light);

}
