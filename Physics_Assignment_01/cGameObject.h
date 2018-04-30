#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>

enum eTypeOfObject
{	// Ok people, here's the deal:
	SPHERE = 0,		// - it's a sphere
	PLANE = 1,		// - it's a plane
	CAPSULE = 2,    // - it's a capsule
	AABB = 3,		// 3- it's an axis-aligned box
	UNKNOWN = 99	// I don't know
};

class cGameObject
{
public:
	cGameObject();		// constructor
	~cGameObject();		// destructor
	//Orientation of model in space
	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 orientation2;		// HACK (will elimiate this with)
	float scale;
	bool wireFrame;

	//Values pertaining to physics
	glm::vec3 vel;			// Velocity
	glm::vec3 accel;		// Acceleration
	bool bIsUpdatedInPhysics;
	eTypeOfObject typeOfObject;		// (really an int)
	float radius;

	glm::vec4 diffuseColour;

	std::string meshName;
};

#endif