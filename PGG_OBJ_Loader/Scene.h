
#include "GameObject.h"

// The GLM library contains vector and matrix functions and classes for us to use
// They are designed to easily work with OpenGL!
#include <GLM/glm.hpp> // This is the main GLM header
#include <GLM/gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include <GLM/gtc/type_ptr.hpp> // This one gives us access to a utility function which makes sending data to OpenGL nice and easy

#include <string>

// The scene contains objects, the camera and light
// It is responsible for coordinating these things
class Scene
{
public:

	
	// Currently the scene is set up in the constructor
	// This means the object(s) are loaded, given materials and positions as well as the camera and light
	Scene();
	~Scene();

	// Use these to adjust the camera's orientation
	// Camera is currently set up to rotate about the world-space origin NOT the camera's origin
	void ChangeCameraAngleX( float value ) { _cameraAngleX += value; }
	void ChangeCameraAngleY( float value ) { _cameraAngleY += value; }

	// Calls update on all objects in the scene
	void Update( float deltaTs );

	// Draws the scene from the camera's point of view
	void Draw();

	unsigned int hdrTexture;
	unsigned int envMap;
	unsigned int irrMap;
	unsigned int prefilMap;
	unsigned int brdfTex;
	unsigned int RBO;
	unsigned int FBO;

protected:

	// Currently one object, this could be a list of objects!
	GameObject *_model;
	//GameObject *_model2;
	//GameObject *_model3;
	GameObject *cube;
	GameObject *cube2;
	GameObject *cube3;
	GameObject *cube4;
	GameObject *quad;
	// This matrix represents the camera's position and orientation
	glm::mat4 _viewMatrix;
	//Camera orientation for equirectangular cubemap
	glm::mat4 _cubeviewMatrix[999]= {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	
	// This matrix is like the camera's lens
	glm::mat4 _projMatrix;
	//setting up projection matrix for equirectangular cubemap
	glm::mat4 _cubeprojMatrix;


	// Current rotation information about the camera
	float _cameraAngleX, _cameraAngleY;

	// Position of the single point-light in the scene
	glm::vec3 _lightPosition;


};
