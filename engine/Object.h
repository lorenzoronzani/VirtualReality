//Tolto il warning riguardante l'export delle STL nella dll
#pragma warning(disable:4251)
#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>

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

class LIB_API Object {
public:
	Object();
	virtual ~Object();

	int id() const;

	void name(std::string name);
	std::string name() const;

	//virtual void render(std::shared_ptr<Object> camera) = 0;
	virtual void render(glm::mat4 modelView) = 0;

private:
	const int m_id;
	std::string m_name;
};
