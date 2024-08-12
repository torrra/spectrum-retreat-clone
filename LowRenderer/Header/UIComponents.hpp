#pragma once

#include <string>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"

class UIComponents
{
public:
	UIComponents(void) = default;
	~UIComponents(void) = default;

	static void CameraTab(Camera* camera)
	{
		// Call UI components 
		camera->CameraUI();
	}
	static void LightingTab(std::vector<PointLight*> point, std::vector<DirectionalLight*> direction, std::vector<SpotLight*> spot)
	{
		// Give each light a different id
		int id = 0;
		for (PointLight* pointLight : point)
		{
			// Create UI section for each different point light
			pointLight->PointLightUI(id);

			// Increment ID in order for a different numerical ID for each light
			++id;
		}

		id = 0;
		for (DirectionalLight* directionalLight : direction)
		{
			// Create UI section for each different directional light
			directionalLight->DirectionalUI(id);
			
			// Increment ID in order for a different numerical ID for each light
			++id;
		}

		id = 0;
		for (SpotLight* spotLight : spot)
		{
			// Create UI section for each different spot light
			spotLight->SpotlightUI(id);
			
			// Increment ID in order for a different numerical ID for each light
			++id;
		}
	}
};