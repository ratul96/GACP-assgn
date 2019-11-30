#define STB_IMAGE_IMPLEMENTATION
#include "Scene.h"

#include <iostream>
#include <SDL/SDL.h>
#include "stb_image.h"


Scene::Scene()
{
	_model = new GameObject();
	//_model2 = new GameObject();
	//_model3 = new GameObject();
	cube = new GameObject();
	cube2 = new GameObject();
	cube3 = new GameObject();
	cube4 = new GameObject();
	quad = new GameObject();

	// Create the material for the game object
	Material *modelMaterial = new Material();
	Material *modelMaterial2 = new Material();
	Material *modelMaterial3 = new Material();
	Material *cubeMaterial = new Material();
	Material *cubeMaterial2 = new Material();
	Material *cubeMaterial3 = new Material();
	Material *cubeMaterial4 = new Material();
	Material *quadMaterial = new Material();


	_cameraAngleX = 0.3f, _cameraAngleY = 1.5f;

	// Set up the viewing matrix
	// This represents the camera's orientation and position
	_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-3.5f) );
	

	// Set up a projection matrix
	_projMatrix = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
	
	_cubeprojMatrix= glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	// Set up your scene here
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO); // Setting up the buffers
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
	//unsigned int lastTime = SDL_GetTicks();
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf("Mono_Lake_B_Ref.hdr", &width, &height, &nrComponents, 0);
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // Loading HDR image

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}


	glGenTextures(1, &envMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);
	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //generating equivalent cubemap for HDR environment
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	cubeMaterial->LoadShaders("HDRVertexshader.txt", "HDRFragshader.txt");
	glUniform1f(glGetUniformLocation(cubeMaterial->_shaderProgram, "equirectangularMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	cube->SetMaterial(cubeMaterial);
	Mesh *cubeMesh = new Mesh();
	cubeMesh->LoadCube();
	cube->SetMesh(cubeMesh);

	//glViewport(0, 0, 512, 512);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for (unsigned int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cube->Draw(_cubeviewMatrix[i], _cubeprojMatrix);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glGenTextures(1, &irrMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irrMap);
	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); //generating cubemap for HDR environment convolution for Irradiance
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	cubeMaterial3->LoadShaders("HDRVertexshader.txt", "IRFragShader.txt");

	glUniform1f(glGetUniformLocation(cubeMaterial3->_shaderProgram, "envMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);

	cube3->SetMaterial(cubeMaterial3);
	Mesh *cubeMesh3 = new Mesh();
	cubeMesh3->LoadCube();
	cube3->SetMesh(cubeMesh3);
	//glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	for (unsigned int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irrMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cube3->Draw(_cubeviewMatrix[i], _cubeprojMatrix);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	

	glGenTextures(1, &prefilMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //generating new cubemap for storing prefiletered environment data
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);                 // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	
	cubeMaterial4->LoadShaders("HDRVertexshader.txt", "prefilterFragShader.txt");
	glUniform1f(glGetUniformLocation(cubeMaterial4->_shaderProgram, "envMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);

	cube4->SetMaterial(cubeMaterial4);
	Mesh *cubeMesh4 = new Mesh();
	cubeMesh4->LoadCube();
	cube4->SetMesh(cubeMesh4);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	unsigned int maxMip = 5;
	for (unsigned int mip = 0; mip < maxMip; mip++)
	{
		unsigned int mipW = 128 * std::pow(0.5, mip);
		unsigned int mipH = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipW, mipH);
		//glViewport(0, 0, mipW, mipH);

		float roughness = (float)mip / (float)(maxMip - 1);
		glUniform1f(glGetUniformLocation(cubeMaterial4->_shaderProgram, "roughness"), roughness);
		for (unsigned int i = 0; i < 6; i++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cube4->Draw(_cubeviewMatrix[i], _cubeprojMatrix);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &brdfTex);
	// pre-allocating enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfTex, 0);

	quadMaterial->LoadShaders("brdfVertShader.txt", "brdfFragShader.txt");
	quad->SetMaterial(quadMaterial);
	Mesh* quadMesh = new Mesh();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quadMesh->LoadQuad();
	quadMesh->DrawQuad();
	quad->SetMesh(quadMesh);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	


	
	// Position of the light, in world-space
	//_lightPosition = glm::vec3(1.2f, 2.0f, 1.2f);
	//Ambient Color
	//GLfloat ambientColor[] = { 0.2f,0.2f,0.2f,1.0f };
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	// Create a game object
	// This needs a material and a mesh
	

	// Shaders are now in files
	modelMaterial->LoadShaders("VertShader.txt","FragShader.txt");
	//modelMaterial2->LoadShaders("VertShader.txt", "FragShader.txt");
	//modelMaterial3->LoadShaders("VertShader.txt", "FragShader.txt");
	

	

	
	
	cubeMaterial2->LoadShaders("SkyVertShader.txt", "SkyFragShader.txt");

	glUniform1f(glGetUniformLocation(cubeMaterial2->_shaderProgram, "envMap"), 0);
	

	

	


	
	//Set Textures
	
	// Need to tell the material the light's position
	// If you change the light's position you need to call this again
	//modelMaterial->SetLightPosition(_lightPosition);
	// Tell the game object to use this material
	_model->SetMaterial(modelMaterial);
	//_model2->SetMaterial(modelMaterial2);
	//_model3->SetMaterial(modelMaterial3);
	
	cube2->SetMaterial(cubeMaterial2);
	
	

	// The mesh is the geometry for the object
	Mesh *modelMesh = new Mesh();
	Mesh *modelMesh2 = new Mesh();
	Mesh *modelMesh3 = new Mesh();
	
	Mesh *cubeMesh2 = new Mesh();
	
	
	// Load from OBJ file. This must have triangulated geometry
	modelMesh->LoadOBJ("teapot3.obj");
	//modelMesh2->LoadOBJ("teapot3.obj");
	//modelMesh3->LoadOBJ("teapot3.obj");
	
	cubeMesh2->LoadCube();
	
	

	// Tell the game object to use this mesh
	_model->SetMesh(modelMesh);
	//_model2->SetMesh(modelMesh2);
	//_model3->SetMesh(modelMesh3);
	
	cube2->SetMesh(cubeMesh2);
	
	

	_model->SetPosition(10.0f, 10.0f, 10.0f);
	//_model2->SetPosition(-19.0f, -1.0f, -1.5f);
	//_model3->SetPosition(-0.0f, -0.0f, -10.0f);


	
	

	

	
	
	
	
	//glGenTextures(1, &brdfTex);

	

}

Scene::~Scene()
{
	// You should neatly clean everything up here
}

void Scene::Update(float deltaTs)
{
	// Update the game object (this is currently hard-coded to rotate)
	_model->Update(deltaTs);
	//_model2->Update(deltaTs);
	//_model3->Update(deltaTs);
	// This updates the camera's position and orientation
	_viewMatrix = glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.5f)), _cameraAngleX, glm::vec3(1, 0, 0)), _cameraAngleY, glm::vec3(0, 1, 0));
	
	
}

void Scene::Draw()
{
	// Draw that model, giving it the camera's position and projection
	_model->Draw(_viewMatrix, _projMatrix);
	//_model2->Draw(_viewMatrix, _projMatrix);
	//_model3->Draw(_viewMatrix, _projMatrix);
	
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, irrMap);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, prefilMap);

	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, brdfTex);


	
	cube2->Draw(_viewMatrix, _projMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, irrMap);
	//(GL_TEXTURE_CUBE_MAP, prefilMap);



	

}







