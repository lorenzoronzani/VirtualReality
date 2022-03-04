#pragma once
#include "Node.h"

class LIB_API GraphFinder {
public:
	std::shared_ptr<Node LIB_API> findGraph(std::shared_ptr<Node> node, const std::string name);
private:
	void find(std::shared_ptr<Node> node, const std::string name);
	std::shared_ptr<Node> find_graph;
};
