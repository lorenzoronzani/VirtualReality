#pragma once
#include <string>
#include <functional>
#include <array>

#include "Node.h"
#include "Camera.h"
#include "List.h"
#include "LeapHand.h"


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

class LIB_API Engine {
public:
	struct Handler {
		std::function<void(int, int, int)> special;
		std::function<glm::vec3(unsigned char, int, int)> keyboard;
		std::function<void(int, int)> mouse;
		std::function<void()> close;
		int width;
		int height;
		std::array<std::string, 6> skybox_data;
		std::shared_ptr<LeapHand> leap;
	};

	static bool init(Handler p_handler);

	static void clear();

	static void render(const List& list, std::shared_ptr<Camera> camera);
	
	static void updateLeap(std::shared_ptr<Node> node);

	static void swap();

	static std::shared_ptr<Node> load(std::string file);

	static void free();

	static void update();

	static void drawText(const std::string& text, float x, float y);

private:
	Engine(Engine& other) = delete;
	void operator=(const Engine&) = delete;
	Engine() = delete;
	~Engine() = delete;
};
