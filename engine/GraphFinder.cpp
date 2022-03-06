#include "GraphFinder.h"

std::shared_ptr<Node LIB_API> GraphFinder::findGraph(std::shared_ptr<Node> node, const std::string name)
{
    find(node, name);

    return find_graph;
}

void LIB_API GraphFinder::find(std::shared_ptr<Node> node, const std::string name)
{
    //Ricerca ricorsiva all'interno del grafo di scena
    for (int i = 0; i < node->getNumberOfChildren(); i++) {
        
        if (node->getChild(i)->name() == name) {
            find_graph = node->getChild(i);
        }
        else {
            find(node->getChild(i), name);
        }
    }
}
