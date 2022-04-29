#include "Node.h"
#include "GraphFinder.h"

LIB_API Node::Node() : m_matrix{ glm::mat4(1.0f) } {

}

LIB_API Node::~Node() {

}

void LIB_API Node::addChild(NodePtr child) {
    m_children.push_back(child);
}

void LIB_API Node::removeChild(int num) {
    for (int i = 0; i < m_children.size(); i++) {
        if (m_children.at(i)->id() == num) {
            m_children.erase(m_children.begin()+i);
        }
    }
}

void LIB_API Node::parent(NodePtr parent) {
    m_parent = parent;
}

Node::NodePtr LIB_API Node::parent() const
{
    return m_parent;
}

void LIB_API Node::removeParent() {
    m_parent = nullptr;
}

Node::NodePtr LIB_API Node::getChild(int num) const {
    return m_children.at(num);
}

Node::NodePtr LIB_API Node::getChildByName(const std::string& name)
{
    GraphFinder graph;
    return graph.findGraph(std::make_shared<Node>(*this),name);
}

int LIB_API Node::getNumberOfChildren() const{
    return (int)m_children.size();
}

void LIB_API Node::render(glm::mat4 modelView, ShaderSettings &shader) {

}

void LIB_API Node::setTransformation(const glm::mat4& transformation) {
    m_matrix = transformation;
}

glm::mat4 LIB_API Node::getFinalMatrix() const{
    return m_matrix;
}

Node LIB_API* Node::clone()
{
    return new Node(*this);
}
