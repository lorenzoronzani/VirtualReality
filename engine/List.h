#pragma once
#include <deque>
#include <memory>
#include <map>
#include "Object.h"
#include "Light.h"
#include "Mesh.h"

class LIB_API List{
public:
	List() = default;
	~List() = default;

	void add(std::shared_ptr<Node> object);
	
	int size() const;
	
	const std::shared_ptr<Node> operator[](std::size_t i) const;
	
	void pass(std::shared_ptr<Node> node);
	
	std::shared_ptr<Node> getByName(const std::string& name);
private:
	std::deque<std::shared_ptr<Node>> m_list;
	std::map<std::string, std::shared_ptr<Node>> m_map;
};

