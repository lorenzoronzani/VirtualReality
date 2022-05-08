#include "Test.h"
#include "engine.h"

void LIB_API Test::testAll() {
	List list;

	//Check lista
	list.add(std::make_shared<Light>(), glm::mat4(1.0f));
	list.add(std::make_shared<Mesh>(), glm::mat4(1.0f));
	list.add(std::make_shared<Light>(), glm::mat4(1.0f));
	list.add(std::make_shared<Mesh>(), glm::mat4(1.0f));

	assert(dynamic_cast<Light*>(list[0].first.get()));
	assert(dynamic_cast<Light*>(list[1].first.get()));
	assert(dynamic_cast<Mesh*>(list[2].first.get()));
	assert(dynamic_cast<Mesh*>(list[3].first.get()));
	std::cout << "List: ok" << std::endl;

	//Check load scene
	auto node = Engine::load("test/cube.OVO");
	std::cout << "Load from file: ok" << std::endl;

	//Check load material
	auto plane = node->getChildByName("Plane001");
	assert(dynamic_cast<Mesh*>(plane.get())->material() != nullptr);
	std::cout << "Material loaded: ok" << std::endl;

	//Check node search
	assert(node != nullptr);
	assert(plane != nullptr);
	std::cout << "Search node: ok" << std::endl;

	//Check remove from list
	auto size = node->getNumberOfChildren();
	node->removeChild(node->getChildByName("Plane001")->id());

	auto size_remove = node->getNumberOfChildren();
	assert(size-1 == size_remove);
	std::cout << "Remove: ok" << std::endl;

	//Check add node to list
	node->addChild(std::make_shared<Node>());
	std::cout << "Add node: ok" << std::endl;
	assert(node->getNumberOfChildren() == size_remove + 1);

	List list_node;

	//Check pass list
	list_node.pass(node);
	std::cout << "List pass: ok" << std::endl;

	//Check get matrix from list
	auto sphere=list_node.getByName("Sphere001");
	auto sphere_init_matrix=sphere.first->getFinalMatrix();

	glm::mat4 sphere_matrix = { {0,0,-2,0} ,{1.96,0,0,0} ,{0,-2,0,0} ,{-0.9,12.55,-1,1} };

	for (int i = 0; i < sphere_matrix.length(); i++) {
		for (int j = 0; j < sphere_matrix[0].length(); j++) {
			assert(static_cast<int>(sphere_init_matrix[i][j]) == static_cast<int>(sphere_matrix[i][j]));
		}
	}
	std::cout << "Matrix list moved correctly: ok" << std::endl;
}
