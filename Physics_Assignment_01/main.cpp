#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "SceneHandler.h"
#include "cShaderManager.h"
#include "cMesh.h"
#include "cVAOMeshManager.h"
#include "cGameObject.h"
#include "cLightManager.h"
#include "Physics.h"

//Camera variables
float angle = 0.0f;
float lookx = 0.0f, looky = 3.0f, lookz = -1.0f;
float camPosx = 0.0f, camPosz = 30.0f;
glm::vec3 g_cameraXYZ = glm::vec3(0.0f, 3.0f, 20.0f);
glm::vec3 g_cameraTarget_XYZ = glm::vec3(0.0f, 3.0f, 0.0f);

//Global handlers for shader, VAOs, game objects and lights
cShaderManager* g_pShaderManager = new cShaderManager();
cVAOMeshManager* g_pVAOManager = new cVAOMeshManager();
std::vector <cMesh> g_vecMesh;
std::vector <cGameObject*> g_vecGameObject;
std::vector <glm::vec3> g_vecObjStart;
cLightManager* g_pLightManager = new cLightManager();

void PhysicsStep(double);

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	//Space turns the blue guy (or whatever models was loaded second) into a wireframe and back
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		::g_vecGameObject[1]->wireFrame = !::g_vecGameObject[1]->wireFrame;
	
	//Keys 1 through 8 will turn on and off the 8 point lights on the scene
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[0].lightIsOn = !(::g_pLightManager->vecLights[0].lightIsOn);
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[1].lightIsOn = !(::g_pLightManager->vecLights[1].lightIsOn);
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[2].lightIsOn = !(::g_pLightManager->vecLights[2].lightIsOn);
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[3].lightIsOn = !(::g_pLightManager->vecLights[3].lightIsOn);
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[4].lightIsOn = !(::g_pLightManager->vecLights[4].lightIsOn);
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[5].lightIsOn = !(::g_pLightManager->vecLights[5].lightIsOn);
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[6].lightIsOn = !(::g_pLightManager->vecLights[6].lightIsOn);
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
		::g_pLightManager->vecLights[7].lightIsOn = !(::g_pLightManager->vecLights[7].lightIsOn);

	const float CAMERASPEED = 0.03f;
	switch (key)
	{
	//MOVEMENT USING TANK CONTROLS WASD
	case GLFW_KEY_A:		// Look Left
		angle -= CAMERASPEED;
		lookx = sin(angle);
		lookz = -cos(angle);
		break;
	case GLFW_KEY_D:		// Look Right
		angle += CAMERASPEED;
		lookx = sin(angle);
		lookz = -cos(angle);
		break;
	case GLFW_KEY_W:		// Move Forward (relative to which way you're facing)
		camPosx += lookx * 0.1f;
		camPosz += lookz * 0.1f;
		break;
	case GLFW_KEY_S:		// Move Backward
		camPosx -= lookx * 0.1f;
		camPosz -= lookz * 0.1f;
		break;
	case GLFW_KEY_DOWN:		// Look Down (along y axis)
		//if (g_cameraTarget_XYZ.y > 0.5)	//Up and down looking range is limited
			looky -= CAMERASPEED;
		break;
	case GLFW_KEY_UP:		// Look Up (along y axis)
		//if (g_cameraTarget_XYZ.y < 1.5)
			looky += CAMERASPEED;
		break;
	case GLFW_KEY_I:
		g_vecGameObject[4]->vel.z -= 0.1;
		break;
	case GLFW_KEY_K:
		g_vecGameObject[4]->vel.z += 0.1;
		break;
	case GLFW_KEY_J:
		g_vecGameObject[4]->vel.x -= 0.1;
		break;
	case GLFW_KEY_L:
		g_vecGameObject[4]->vel.x += 0.1;
		break;
	}
}

int main()
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1080, 720,
		"Balls Party",
		NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";

	::g_pShaderManager->setBasePath("assets//shaders//");

	if (!::g_pShaderManager->createProgramFromFile(
		"simpleShader", vertShader, fragShader))
	{
		std::cout << "Failed to create shader program. Shutting down." << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		return -1;
	}
	std::cout << "The shaders comipled and linked OK" << std::endl;
	GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName("simpleShader");

	std::string* plyDirects;
	std::string* plyNames;
	int numPlys;

	if (!readPlysToLoad(&plyDirects, &plyNames, &numPlys))
	{
		std::cout << "Couldn't find files to read. Giving up hard.";
		return -1;
	}

	for (int i = 0; i < numPlys; i++)
	{	//Load each file into a VAO object
		cMesh newMesh;
		newMesh.name = plyNames[i];
		if (!LoadPlyFileIntoMesh(plyDirects[i], newMesh))
		{
			std::cout << "Didn't load model" << std::endl;
		}
		g_vecMesh.push_back(newMesh);
		if (!::g_pVAOManager->loadMeshIntoVAO(newMesh, shaderID))
		{
			std::cout << "Could not load mesh into VAO" << std::endl;
		}
	}

	int numEntities;
	if (!readEntityDetails(&g_vecGameObject, &numEntities, &g_vecObjStart))
	{
		std::cout << "Unable to find game objects for placement." << std::endl;
	}


	std::cout << glGetString(GL_VENDOR) << " "
		<< glGetString(GL_RENDERER) << ", "
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("simpleShader");

	GLint uniLoc_mModel = glGetUniformLocation(currentProgID, "mModel");
	GLint uniLoc_mView = glGetUniformLocation(currentProgID, "mView");
	GLint uniLoc_mProjection = glGetUniformLocation(currentProgID, "mProjection");

	GLint uniLoc_materialDiffuse = glGetUniformLocation(currentProgID, "materialDiffuse");

	::g_pLightManager->createLights();	// There are 10 lights in the shader
	::g_pLightManager->LoadShaderUniformLocations(currentProgID);

	glEnable(GL_DEPTH);

	double lastTimeStep = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		glm::mat4x4 m, p;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		::g_pLightManager->CopyInfoToShader();

		unsigned int sizeOfVector = ::g_vecGameObject.size();	//*****//
		for (int index = 0; index != sizeOfVector; index++)
		{
			// Is there a game object? 
			if (::g_vecGameObject[index] == 0)	//if ( ::g_GameObjects[index] == 0 )
			{	// Nothing to draw
				continue;		// Skip all for loop code and go to next
			}

			// Was near the draw call, but we need the mesh name
			std::string meshToDraw = ::g_vecGameObject[index]->meshName;		//::g_GameObjects[index]->meshName;

			sVAOInfo VAODrawInfo;
			if (::g_pVAOManager->lookupVAOFromName(meshToDraw, VAODrawInfo) == false)
			{	// Didn't find mesh
				continue;
			}

			m = glm::mat4x4(1.0f);	

			glm::mat4 matPreRotX = glm::mat4x4(1.0f);
			matPreRotX = glm::rotate(matPreRotX, ::g_vecGameObject[index]->orientation.x,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPreRotX;
			
			glm::mat4 matPreRotY = glm::mat4x4(1.0f);
			matPreRotY = glm::rotate(matPreRotY, ::g_vecGameObject[index]->orientation.y,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPreRotY;
			
			glm::mat4 matPreRotZ = glm::mat4x4(1.0f);
			matPreRotZ = glm::rotate(matPreRotZ, ::g_vecGameObject[index]->orientation.z,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPreRotZ;

			glm::mat4 trans = glm::mat4x4(1.0f);
			trans = glm::translate(trans,
				::g_vecGameObject[index]->position);
			m = m * trans;

			glm::mat4 matPostRotZ = glm::mat4x4(1.0f);
			matPostRotZ = glm::rotate(matPostRotZ, ::g_vecGameObject[index]->orientation2.z,
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * matPostRotZ;

			glm::mat4 matPostRotY = glm::mat4x4(1.0f);
			matPostRotY = glm::rotate(matPostRotY, ::g_vecGameObject[index]->orientation2.y,
				glm::vec3(0.0f, 1.0f, 0.0f));
			m = m * matPostRotY;


			glm::mat4 matPostRotX = glm::mat4x4(1.0f);
			matPostRotX = glm::rotate(matPostRotX, ::g_vecGameObject[index]->orientation2.x,
				glm::vec3(1.0f, 0.0f, 0.0f));
			m = m * matPostRotX;

			float finalScale = ::g_vecGameObject[index]->scale;
			glm::mat4 matScale = glm::mat4x4(1.0f);
			matScale = glm::scale(matScale,
				glm::vec3(finalScale,
						finalScale,
						finalScale));
			m = m * matScale;

			p = glm::perspective(0.6f,			// FOV
				ratio,		// Aspect ratio
				0.1f,			// Near (as big as possible)
				1000.0f);	// Far (as small as possible)

							// View or "camera" matrix
			glm::mat4 v = glm::mat4(1.0f);	// identity

			g_cameraXYZ.x = camPosx;
			g_cameraXYZ.z = camPosz;

			g_cameraTarget_XYZ.x = camPosx + lookx;
			g_cameraTarget_XYZ.y = looky;
			g_cameraTarget_XYZ.z = camPosz + lookz;

			v = glm::lookAt(g_cameraXYZ,						// "eye" or "camera" position
				g_cameraTarget_XYZ,		// "At" or "target" 
				glm::vec3(0.0f, 1.0f, 0.0f));	// "up" vector


			glUniform4f(uniLoc_materialDiffuse,
				::g_vecGameObject[index]->diffuseColour.r,
				::g_vecGameObject[index]->diffuseColour.g,
				::g_vecGameObject[index]->diffuseColour.b,
				::g_vecGameObject[index]->diffuseColour.a);


			//        glUseProgram(program);
			::g_pShaderManager->useShaderProgram("simpleShader");

			glUniformMatrix4fv(uniLoc_mModel, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(m));

			glUniformMatrix4fv(uniLoc_mView, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(v));

			glUniformMatrix4fv(uniLoc_mProjection, 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(p));

			if(::g_vecGameObject[index]->wireFrame)
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBindVertexArray(VAODrawInfo.VAO_ID);

			glDrawElements(GL_TRIANGLES,
				VAODrawInfo.numberOfIndices,
				GL_UNSIGNED_INT,
				0);

			glBindVertexArray(0);

		}//for ( int index = 0...

		double curTime = glfwGetTime();
		double deltaTime = curTime - lastTimeStep;

		PhysicsStep(deltaTime);

		lastTimeStep = curTime;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void PhysicsStep(double deltaTime)
{
	const glm::vec3 GRAVITY = glm::vec3(0.0f, -2.0f, 0.0f);
	for (int index = 0; index != ::g_vecGameObject.size(); index++)
	{
		cGameObject* pCurGO = ::g_vecGameObject[index];

		// Is this object to be updated?
		if (!pCurGO->bIsUpdatedInPhysics)
		{	// DON'T update this
			continue;		// Skip everything else in the for
		}

		// Explicit Euler  (RK4) 
		// New position is based on velocity over time
		glm::vec3 deltaPosition = (float)deltaTime * pCurGO->vel;
		pCurGO->position += deltaPosition;

		// New velocity is based on acceleration over time
		glm::vec3 deltaVelocity = ((float)deltaTime * pCurGO->accel)
			+ ((float)deltaTime * GRAVITY);

		pCurGO->vel += deltaVelocity;

		switch (pCurGO->typeOfObject)
		{
		case eTypeOfObject::SPHERE:
			// Compare this to EVERY OTHER object in the scene
			for (int indexEO = 0; indexEO != ::g_vecGameObject.size(); indexEO++)
			{
				// Don't test for myself
				if (index == indexEO)
					continue;	// It's me!! 

				cGameObject* pOtherObject = ::g_vecGameObject[indexEO];
				// Is Another object
				switch (pOtherObject->typeOfObject)
				{
				case eTypeOfObject::SPHERE:
					// 
					if (PenetrationTestSphereSphere(pCurGO, pOtherObject))
					{
						float m1, m2, x1, x2;
						glm::vec3 v1temp, v1, v2, v1x, v2x, v1y, v2y, x(pCurGO->position - pOtherObject->position);

						x = glm::normalize(x);
						v1 = pCurGO->vel;
						x1 = glm::dot(x, v1);
						v1x = x * x1;
						v1y = v1 - v1x;
						m1 = 1.0f; //Mass of the sphere. Set mass of all spheres to 1, just to simplify...

						x = -x;

						v2 = pOtherObject->vel;
						x2 = glm::dot(x, v2);
						v2x = x * x2;
						v2y = v2 - v2x;
						m2 = 1.0f;

						pCurGO->vel = glm::vec3(v1x*(m1 - m2) / (m1 + m2) + v2x*(2 * m2) / (m1 + m2) + v1y);
						pOtherObject->vel = glm::vec3(v1x*(2 * m1) / (m1 + m2) + v2x*(m2 - m1) / (m1 + m2) + v2y);
					}
					break;

				case eTypeOfObject::PLANE:
					cMesh thisMesh;
					for (int i = 0; i < ::g_vecMesh.size(); i++)
					{
						if (pOtherObject->meshName == ::g_vecMesh[i].name)
						{
							thisMesh = ::g_vecMesh[i];
							break;
						}
					}
					for (int i = 0; i < thisMesh.numberOfTriangles; i++)
					{
						int ptA = thisMesh.pTriangles[i].vertex_ID_0;
						int ptB = thisMesh.pTriangles[i].vertex_ID_1;
						int ptC = thisMesh.pTriangles[i].vertex_ID_2;

						glm::vec3 vertexA = glm::vec3(thisMesh.pVertices[ptA].x, thisMesh.pVertices[ptA].y, thisMesh.pVertices[ptA].z);
						glm::vec3 vertexB = glm::vec3(thisMesh.pVertices[ptB].x, thisMesh.pVertices[ptB].y, thisMesh.pVertices[ptB].z);
						glm::vec3 vertexC = glm::vec3(thisMesh.pVertices[ptC].x, thisMesh.pVertices[ptC].y, thisMesh.pVertices[ptC].z);
						glm::vec3 spherePos = glm::vec3(pCurGO->position.x, pCurGO->position.y, pCurGO->position.z);

						glm::vec3 closestPoint = ClosestPtPointTriangle(spherePos, vertexA, vertexB, vertexC);

						float distance = glm::distance(pCurGO->position, closestPoint);

						if (distance <= pCurGO->radius)
						{	//THIS IS A COLLISION
							glm::vec3 planeNormal = glm::vec3(thisMesh.pVertices[ptA].nx, thisMesh.pVertices[ptA].ny, thisMesh.pVertices[ptA].nz);
							
							glm::vec3 reflection = 2.0f * planeNormal * (planeNormal * pCurGO->vel);
								pCurGO->vel -= reflection;

							break;
						}
					}
					break;

				}//switch ( pOtherObject->typeOfObject )

			}
			break;
		};

		if (pCurGO->position.y < -30.0f)
		{
			pCurGO->position = g_vecObjStart[index];
			pCurGO->vel = glm::vec3(0.0f);
			pCurGO->accel = glm::vec3(0.0f);
		}



	}//for ( int index...



	return;


}