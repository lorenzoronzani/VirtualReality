#include "Test.h"
#include "engine.h"
LIB_API Test::Test() {

}

LIB_API Test::~Test() {

}

void LIB_API Test::testAll() {
	List list;
	list.add(std::make_shared<Light>());
	list.add(std::make_shared<Mesh>());
	list.add(std::make_shared<Light>());
	list.add(std::make_shared<Mesh>());
	assert(dynamic_cast<Light*>(list[0].get()));
	assert(dynamic_cast<Light*>(list[1].get()));
	assert(dynamic_cast<Mesh*>(list[2].get()));
	assert(dynamic_cast<Mesh*>(list[3].get()));
	List list_node;
	std::cout << "List: ok" << std::endl;
	auto node = Engine::load("test/cube.OVO");
	std::cout << "Load from file: ok" << std::endl;
	auto plane = node->getChildByName("Plane001");
	assert(dynamic_cast<Mesh*>(plane.get())->material() != nullptr);
	std::cout << "Material loaded: ok" << std::endl;
	assert(node != nullptr);
	assert(plane != nullptr);
	std::cout << "Search node: ok" << std::endl;
	auto size = node->getNumberOfChildren();
	node->removeChild(node->getChildByName("Plane001")->id());
	auto size_remove = node->getNumberOfChildren();
	assert(size-1 == size_remove);
	std::cout << "Remove: ok" << std::endl;
	node->addChild(std::make_shared<Node>());
	std::cout << "Add node: ok" << std::endl;
	assert(node->getNumberOfChildren() == size_remove + 1);
	list_node.pass(node);
	std::cout << "List pass: ok" << std::endl;
	auto sphere=list_node.getByName("Sphere001");
	auto sphere_init_matrix=sphere->getFinalMatrix();
	glm::mat4 sphere_matrix = { {0,0,-2,0} ,{1.96,0,0,0} ,{0,-2,0,0} ,{-0.9,12.55,-1,1} };
	for (int i = 0; i < sphere_matrix.length(); i++) {
		for (int j = 0; j < sphere_matrix[0].length(); j++) {
			assert(static_cast<int>(sphere_init_matrix[i][j]) == static_cast<int>(sphere_matrix[i][j]));
		}
	}
	std::cout << "Matrix list moved correctly: ok" << std::endl;
}
