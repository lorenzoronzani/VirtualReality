#pragma once

#include "Node.h"

#include <vector>   
#include <iostream>
#include <iomanip>   
#include <limits.h> 
#include <fstream>
#include <map>
#include <string>

#include "Material.h"
#include "Mesh.h"
#include "Light.h"

class LIB_API ObjectLoader {
private:
	struct NodeType {
		unsigned int length;
		std::shared_ptr<Node> node;
		std::shared_ptr<Material> material;
	};
    
    class OvObject{
    public:
        enum class Type : int{
            OBJECT = 0,
            NODE,
            OBJECT2D,
            OBJECT3D,
            LIST,
            BUFFER,
            SHADER,
            TEXTURE,
            FILTER,
            MATERIAL,
            FBO,
            QUAD,
            BOX,
            SKYBOX,
            FONT,
            CAMERA,
            LIGHT,
            BONE,
            MESH,	   
            SKINNED,   
            INSTANCED,
            PIPELINE,
            EMITTER,
            ANIM,
            PHYSICS,
            LAST,
        };
    };
    
    class OvMesh{
    public:
        enum class Subtype : int{
            DEFAULT = 0,
            NORMALMAPPED,
            TESSELLATED,
            LAST,
        };
    };
	
    std::vector<char> readFile(std::string filepath);
	
    NodeType readNode(char* data, int chunkId);
	
    void readMaterials(char* buffer, unsigned int& position);
	
    std::shared_ptr<Node> recursiveLoad(char* buffer, unsigned int& position);
    

    //Variabili
    std::map<std::string, std::shared_ptr<Material>> material;
    int num_light;
    int num_texture;
    std::string m_directory;

public:
	ObjectLoader();
	~ObjectLoader();
	std::shared_ptr<Node> LoadScene(const std::string& file);
};