#include "List.h"

LIB_API List::List(){

}

LIB_API List::~List(){

}

void LIB_API List::add(std::shared_ptr<Node> object){
	m_map[object->name()] = object;
	if (dynamic_cast<Light*>(object.get())) {
		m_list.push_front(object);
	} else if(dynamic_cast<Mesh*>(object.get())) {
		m_list.push_back(object);
	}
}

int LIB_API List::size() const
{
	return m_list.size();
}

const LIB_API std::shared_ptr<Node> List::operator[](std::size_t i) const
{
	return m_list.at(i);
}

void LIB_API List::pass(std::shared_ptr<Node> node)
{
	for (int i = 0; i < node->getNumberOfChildren(); i++) {
		//virtual copy pattern
		std::shared_ptr<Node> current(node->getChild(i)->clone());
		current->setTransformation(node->getFinalMatrix() * current->getFinalMatrix());
		add(current);
		pass(current);
	}
}

std::shared_ptr<Node LIB_API> List::getByName(const std::string& name)
{
	return m_map[name];
}
