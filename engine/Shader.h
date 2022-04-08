/**
 * @file		shader.h
 * @brief	Minimal OpenGL shader class
 *
 * @author	Achille Peternier (C) SUPSI [achille.peternier@supsi.ch]
 */
#pragma once



 //////////////
 // #INCLUDE //
 //////////////

	// GLM:
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// C/C++:
#include <iostream>

#ifdef _WINDOWS
// Export API:
// Specifies i/o linkage (VC++ spec):
#ifdef ENGINE_EXPORTS
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else // Under Linux
#define LIB_API  // Dummy declaration
#endif

//////////////////
// CLASS Shader //
//////////////////

/**
 * @brief OGL Shader. Management and rendering of shaders.
 */
class LIB_API Shader {
	//////////
public: //
//////////

   // Constants:
	static const unsigned int MAX_LOGSIZE = 4096;  ///< Max output size in char for a shader log

	 // Enums:
	enum ///< Kind of shader
	{
		TYPE_UNDEFINED = 0,
		TYPE_VERTEX,
		TYPE_FRAGMENT,
		TYPE_PROGRAM,
		TYPE_LAST
	};

	// Const/dest:	 
	Shader();
	~Shader();

	// Get/set:
	int getParamLocation(const char* name);
	void setMatrix(int param, const glm::mat4& mat);
	void setFloat(int param, float value);
	void setInt(int param, int value);
	void setVec3(int param, const glm::vec3& vect);
	void setVec4(int param, const glm::vec4& vect);
	void setMatrix3(int param, const glm::mat3& mat);

	// Accessing data:
	bool loadFromMemory(int kind, const char* data);
	bool loadFromFile(int kind, const char* filename);
	bool build(Shader* vertexShader, Shader* fragmentShader);

	void bind(int location, const char* attribName);

	// Rendering:				
	bool render(void* data = nullptr);

	unsigned int glId;

	///////////	 
private:	//
///////////					

   // Object kind:
	int type;

	// OGL id:
};