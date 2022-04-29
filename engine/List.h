#pragma once
#include <deque>
#include <memory>
#include <map>
#include <utility>
#include "Object.h"
#include "Light.h"
#include "Mesh.h"

class LIB_API List{
public:
	List() = default;
	~List() = default;

	void add(std::shared_ptr<Node> object, glm::mat4 posMat);

	int size() const;

	const std::pair<std::shared_ptr<Node>, glm::mat4> operator[](std::size_t i) const;

	void pass(std::shared_ptr<Node> node);

	std::shared_ptr<Node> getByName(const std::string& name);

private:
	std::deque<std::pair<std::shared_ptr<Node>, glm::mat4>> m_list;
	std::map<std::string, std::shared_ptr<Node>> m_map;
	void node_pass(std::shared_ptr<Node> node,glm::mat4 mat);
};

