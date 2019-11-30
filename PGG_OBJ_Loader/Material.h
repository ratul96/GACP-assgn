
#ifndef __MATERIAL__
#define __MATERIAL__

#include <string>
#include <GLM/glm.hpp>
#include "glew.h"
//#include "Scene.h"

// Encapsulates shaders and textures
class Material
{
public:
	Material();
	~Material();

	// Loads shaders from file
	// Returns false if there was an error - it will also print out messages to console
	bool LoadShaders( std::string vertFilename, std::string fragFilename );

	// For setting the standard matrices needed by the shader
	void SetMatrices(glm::mat4 modelMatrix, glm::mat4 invModelMatrix, glm::mat4 viewMatrix, glm::mat4 projMatrix);
	
	// For setting material properties
	void SetEmissiveColour( glm::vec3 input ) { _emissiveColour = input;}
	void SetDiffuseColour( glm::vec3 input ) { _diffuseColour = input;}
	void SetSpecularColour( glm::vec3 input ) { _specularColour = input;}

	// Set light position in world space
	void SetLightPosition( glm::vec3 input ) { _lightPosition = input;}
	unsigned int loadBufferHDR(unsigned int hdrTexture);
	unsigned int loadCubeMap(unsigned int envMap);

	// Sets texture
	// This applies to ambient, diffuse and specular colours
	// If you want textures for anything else, you'll need to do that yourself ;)
	bool SetTexture( std::string filename ) {_texture1 = LoadTexture(filename); return _texture1>0;}
	//bool SetHDR(unsigned int hdrtexture) { _hdrtexture = loadBufferHDR(hdrtexture); return _hdrtexture > 0; }
	//bool SetCubeMap(unsigned int _envMap) { envCubemap = loadCubeMap(_envMap); return envCubemap > 0; }
	

	// Sets the material, applying the shaders
	void Apply();
	int _shaderProgram;
	int _shaderTex2SamplerLocation;
protected:

	// Utility function
	bool CheckShaderCompiled( GLint shader );
	
	// The OpenGL shader program handle
	

	// Locations of Uniforms in the vertex shader
	int _shaderModelMatLocation;
	int _shaderInvModelMatLocation;
	int _shaderViewMatLocation;
	int _shaderProjMatLocation;

	// Location of Uniforms in the fragment shader
	int _shaderDiffuseColLocation, _shaderEmissiveColLocation, _shaderSpecularColLocation;
	int _shaderWSLightPosLocation;
	int _shaderTex1SamplerLocation;
	
	int _shaderTex3SamplerLocation;

	// Local store of material properties to be sent to the shader
	glm::vec3 _emissiveColour, _diffuseColour, _specularColour;
	glm::vec3 _lightPosition;

	// Loads a .bmp from file
	unsigned int LoadTexture( std::string filename );
	
	// OpenGL handle for the texture
	unsigned int _texture1;

	//OpenGL handle for hdr texture
	unsigned int _hdrtexture;
	
	//Scene *handle=new Scene();


	
};




#endif
