#pragma once
#include "Object.h"
#include <vector>
#include <memory>
#include <iostream>
#include <map>
#include "../dependencies/glm/include/glm/glm.hpp"
#include "../dependencies/glm/include/glm/gtc/packing.hpp"
#include "../dependencies/glm/include/glm/gtc/type_ptr.hpp"
#include "../dependencies/glm/include/glm/gtx/string_cast.hpp"



class LIB_API Node : public Object {
protected:
	using NodePtr = std::shared_ptr<Node>;

public:
	Node();
	~Node();

	void addChild(NodePtr child);
	void removeChild(int idChild);

	void parent(NodePtr parent);
	NodePtr parent() const;

	void removeParent();

	NodePtr getChild(int num) const;
	NodePtr getChildByName(const std::string& name);

	int getNumberOfChildren() const;

	//void render(std::shared_ptr<Object> camera);
	void render(glm::mat4 modelView);

	virtual void setTransformation(const glm::mat4& transformation);
	glm::mat4 getFinalMatrix() const;

	virtual Node* clone();

private:
	glm::mat4 m_matrix;
	std::vector<NodePtr> m_children;
	NodePtr m_parent;
};
