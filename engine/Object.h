#pragma once
#include <string>
#include <memory>

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

	virtual void render(std::shared_ptr<Object> camera) = 0;

private:
	const int m_id;
	std::string m_name;
};
