#include "Object.h"

static int newId{ 0 };

LIB_API Object::Object() : m_id{ newId } {
	newId++;
}

LIB_API Object::~Object(){
	
}

int LIB_API Object::id() const{
	return m_id;
}

void LIB_API Object::name(std::string name) {
	m_name = name;
}

std::string LIB_API Object::name() const {
	return m_name;
}
