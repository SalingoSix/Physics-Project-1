#ifndef _SCENEHANDLER_HG_
#define _SCENEHANDLER_HG_

#include <vector>
#include "cGameObject.h"
class cMesh;

bool readPlysToLoad(std::string** myPlys, std::string** plyNames, int* length);

bool readEntityDetails(std::vector <cGameObject*>* myObjs, int* length, std::vector <glm::vec3>*);

void ReadFileToToken(std::ifstream &file, std::string token);

bool LoadPlyFileIntoMesh(std::string filename, cMesh &theMesh);

#endif
