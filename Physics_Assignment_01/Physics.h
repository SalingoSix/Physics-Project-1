#ifndef _Physics_HG_
#define _Physics_HG_

#include <glm/vec3.hpp>

//#include "cGameObject.h"
class cGameObject;

// Our objects are vectors of pointers, so we might as well pass pointers
bool PenetrationTestSphereSphere(cGameObject* pA, cGameObject* pB);

glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);


#endif