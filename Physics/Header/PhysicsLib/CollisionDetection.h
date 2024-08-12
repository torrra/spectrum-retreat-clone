#pragma once

#include "LibMath/Vector/Vector3.h"

// Forward declaration of color class
class Color;

// Create namespace to easily identify physics classes
namespace PhysicsLib
{
	// Store collider types
	enum COLLIDER_TYPE
	{
		BOX,
		SPHERE,
		LIGHT_BOX,
		DOOR,
		COLOR_CUBE,
		IGNORE,
		HOLED,
		TELEPORTER,
		FINAL_TOWER
	};

	// Abstract collider class
	class ICollider
	{
	public:
		// Constructor
				 ICollider(void) = default;

		// Destructor
		virtual ~ICollider(void) = default;

		COLLIDER_TYPE m_type = SPHERE;
	};

	// Forward declaration of SphereCollider class
	class SphereCollider;
	
	// BoxCollider which inherits from collider interface
	class BoxCollider : public ICollider
	{
	public:
		// Constructors
				BoxCollider(void) { m_type = BOX; }
				BoxCollider(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, COLLIDER_TYPE type= BOX);

		// Destructor
		virtual ~BoxCollider() override = default;

		// Update collider transform
		void	SetColliderTransform(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale);

		// Functions for collision detection
		static bool CheckCollision(BoxCollider const& box1, BoxCollider const& box2);
		bool		CheckCollision(BoxCollider const& box2);
		bool		CheckCollision(SphereCollider const& sphereCollider);

		LibMath::Vector3 m_minVertex;
		LibMath::Vector3 m_maxVertex;

		LibMath::Vector3 m_position;
		LibMath::Vector3 m_boxScale;

		bool			 m_enabled = true;
	};

	// SphereCollider class which inherits from collider interface
	class SphereCollider : public ICollider
	{
	public:
		// Constructor
				SphereCollider(void) = default;
				SphereCollider(float const& radius, LibMath::Vector3 const& position);

		// Destructor
		virtual ~SphereCollider(void) override = default;

		// Setter
		void	SetColliderPosition(LibMath::Vector3 const& position);

		// Collision detection functions
		static bool CheckCollision(SphereCollider const& sphere1, SphereCollider const& sphere2);
		static bool CheckCollision(SphereCollider const& sphereCollider, BoxCollider const& boxCollider);
		bool		CheckCollision(SphereCollider const& sphere2);
		bool		CheckCollision(BoxCollider const& boxCollider);

		LibMath::Vector3 m_position;
		float m_radius;
	};

	// ColoredBoxCollider class which inherits from collider interface
	class ColoredBoxCollider : public BoxCollider
	{
	public:
		// Constructor
		ColoredBoxCollider(void) { m_type = COLOR_CUBE; }
		ColoredBoxCollider(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, COLLIDER_TYPE type = COLOR_CUBE);

		// Destructor
		~ColoredBoxCollider(void) = default;

		Color* m_color = nullptr;
	};

	// HoleCollider class which inherits from collider interface
	class HoledCollider : public BoxCollider
	{
	public:
		// Constructor
		HoledCollider(void) { m_type = HOLED; }
		HoledCollider(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, COLLIDER_TYPE type = HOLED);

		// Destructor
		~HoledCollider(void) = default;

		BoxCollider* m_hole = nullptr;
	};

	// Teleporter class which inherits from Box collider class
	class Teleporter : public BoxCollider
	{
	public:
		// Constructor
		Teleporter(void) { m_type = HOLED; }
		Teleporter(LibMath::Vector3 const& position, LibMath::Vector3 const& boxScale, COLLIDER_TYPE type = TELEPORTER);

		// Destructor
		~Teleporter(void) = default;

		Teleporter* m_otherSide = nullptr;
	};
}

using Collider		= PhysicsLib::ICollider;
using SphereBV		= PhysicsLib::SphereCollider;
using BoxBV			= PhysicsLib::BoxCollider;
using ColBoxBV		= PhysicsLib::ColoredBoxCollider;