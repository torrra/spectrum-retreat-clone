#include "LightVolume.h"

LightBox::LightBox(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, PhysicsLib::COLLIDER_TYPE type)
{
	m_type = type;
	m_position = position;
	m_boxScale = boxScale;

	// Calculate box collider vertex min & max positions
	m_minVertex = m_position - m_boxScale;
	m_maxVertex = m_position + m_boxScale;
}

void LightBox::AddLight(BaseLight* light)
{
	// Add light to vector
	m_lights.push_back(light);
	light->m_enabled = false;
}

void LightBox::EnableLights()
{
	// Iterate through all lights & enable
	for (BaseLight* light : m_lights)
		light->m_enabled = true;
}

void LightBox::DisableLights()
{
	// Iterate through all lights & disable
	for (BaseLight* light : m_lights)
		light->m_enabled = false;
}