#pragma once

#include "PhysicsLib/CollisionDetection.h"

#include "Light.h"

class LightBox : public BoxBV
{
public:

	LightBox(void) { m_type = PhysicsLib::LIGHT_BOX; }
	LightBox(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, PhysicsLib::COLLIDER_TYPE type = PhysicsLib::LIGHT_BOX);

	void AddLight(BaseLight* light);

	void EnableLights();
	void DisableLights();

	~LightBox(void) = default;

	std::vector<BaseLight*>  m_lights;
};