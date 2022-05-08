#include "List.h"

void LIB_API List::add(std::shared_ptr<Node> object, glm::mat4 posMat) {
	m_map[object->name()] = std::make_pair(object, posMat);

	//Aggiunge differenziando luci e altre mesh
	if (dynamic_cast<Light*>(object.get())) {
		m_list.push_front(std::make_pair(object, posMat));
	}
	else if (dynamic_cast<Mesh*>(object.get())) {
		m_list.push_back(std::make_pair(object, posMat));
	}
}

int LIB_API List::size() const
{
	return (int)m_list.size();
}

const LIB_API std::pair<std::shared_ptr<Node>, glm::mat4> List::operator[](std::size_t i) const
{
	return m_list.at(i);
}

void LIB_API List::pass(std::shared_ptr<Node> node)
{
	node_pass(node,node->getFinalMatrix());
}

void LIB_API List::node_pass(std::shared_ptr<Node> node,glm::mat4 mat) {
	//Attraversa la lista
	for (int i = 0; i < node->getNumberOfChildren(); i++) {
		//virtual copy pattern
		glm::mat4 posMat = mat * node->getChild(i)->getFinalMatrix();
		add(node->getChild(i), posMat);
		node_pass(node->getChild(i),posMat);
	}
}

std::pair<std::shared_ptr<Node>, glm::mat4> LIB_API List::getByName(const std::string& name)
{
	return m_map[name];
}