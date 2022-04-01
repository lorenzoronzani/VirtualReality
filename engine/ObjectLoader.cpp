#include "ObjectLoader.h"
LIB_API ObjectLoader::ObjectLoader(ShaderSettings& shader) : m_shader{ shader },num_light { 0 }, num_texture{ 1 }{
}

LIB_API ObjectLoader::~ObjectLoader(){
}

std::shared_ptr<Node LIB_API> ObjectLoader::LoadScene(const std::string& file)
{
    std::vector<char> read_file = readFile(file);
    
    unsigned int position;
    position = 0;
    
    readMaterials(read_file.data(), position);
	
    return recursiveLoad(read_file.data(),position);
}

std::vector<char> LIB_API ObjectLoader::readFile(std::string filepath)
{
    m_directory = filepath;
    
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    
    //Mi posiziono all'inizio
    file.seekg(0, std::ios::beg);
    
    std::vector<char> buffer(size);
    
    if (file.read(buffer.data(), size))
    {
        return buffer;
    }
    
    return std::vector<char>();
}

ObjectLoader::NodeType LIB_API ObjectLoader::readNode(char* data, int chunkId)
{
    unsigned int position = 0;
    
    switch ((OvObject::Type)chunkId){
        case OvObject::Type::OBJECT:
        {
            unsigned int versionId;

            memcpy(&versionId, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);

            NodeType node;
            node.length = 0;

            return node;
        }
        break;

        case OvObject::Type::NODE:
        {
            NodeType node;
            node.node = std::make_shared<Node>();
            
            //Leggo e setto nome
            char nodeName[FILENAME_MAX];
            strcpy(nodeName, data + position);
            node.node->name(nodeName);
            
            position += (unsigned int)strlen(nodeName) + 1;
            
            //Leggo e setto matrice
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            node.node->setTransformation(matrix);
            
            position += sizeof(glm::mat4);
            
            unsigned int children;
            memcpy(&children, data + position, sizeof(unsigned int));
            
            position += sizeof(unsigned int);
            
            char targetName[FILENAME_MAX];
            strcpy(targetName, data + position);
            
            position += (unsigned int)strlen(targetName) + 1;
            
            node.length = children;
            
            return node;
        }
        break;
        
        case OvObject::Type::MATERIAL:
        {
            NodeType node;
            
            //Leggo e setto nome material
            char materialName[FILENAME_MAX];
            strcpy(materialName, data + position);
            node.material = std::make_shared<Material>();
            
            position += (unsigned int)strlen(materialName) + 1;
            
            node.material->name(materialName);
            
            MaterialSettings material_settings;
            
            glm::vec3 emission, albedo;
            
            //Leggo e setto emission
            memcpy(&emission, data + position, sizeof(glm::vec3));
            
            std::shared_ptr<Texture> texture;
            texture = std::make_shared<Texture>();
            material_settings.emission = emission;
            
            position += sizeof(glm::vec3);
            
            //Leggo e setto albedo
            memcpy(&albedo, data + position, sizeof(glm::vec3));
            material_settings.ambient = glm::mat3(0.2f) * albedo;
            material_settings.diffuse = glm::mat3(0.6f) * albedo;
            material_settings.specular = glm::mat3(0.4f) * albedo;
            
            position += sizeof(glm::vec3);
            
            float roughness;
            
            //Leggo e setto rugosità
            memcpy(&roughness, data + position, sizeof(float));
            material_settings.roughness = (1 - glm::sqrt(roughness)) * 128;
            
            position += sizeof(float);
            
            float metalness;
            
            //Leggo metallicità
            memcpy(&metalness, data + position, sizeof(float));
            
            position += sizeof(float);
            
            float alpha;
            
            //Leggo canale alpha
            memcpy(&alpha, data + position, sizeof(float));
            
            position += sizeof(float);
            
            //Leggo e setto nome texture
            char textureName[FILENAME_MAX];
            strcpy(textureName, data + position);
            texture->name(textureName);
            
            //Setto id texture
            texture->id(num_texture);
            num_texture = num_texture + 1;
            
            std::string directory = "";
            
            const size_t last_slash_idx = m_directory.rfind('/');
            
            if (std::string::npos != last_slash_idx)
            {
                directory = m_directory.substr(0, last_slash_idx);
            }
            
            //Cerco e carico texture
            directory = directory + "/"+textureName;
            texture->load(directory,m_shader);
            
            position += (unsigned int)strlen(textureName) + 1;
            
            //Leggo Normal Map
            char normalMapName[FILENAME_MAX];
            strcpy(normalMapName, data + position);
            
            position += (unsigned int)strlen(normalMapName) + 1;
            
            //Leggo Height Map
            char heightMapName[FILENAME_MAX];
            strcpy(heightMapName, data + position);
            
            position += (unsigned int)strlen(heightMapName) + 1;
            
            //Leggo Roughness Map
            char roughnessMapName[FILENAME_MAX];
            strcpy(roughnessMapName, data + position);
            
            position += (unsigned int)strlen(roughnessMapName) + 1;
            
            //Leggo Metalness Map
            char metalnessMapName[FILENAME_MAX];
            strcpy(metalnessMapName, data + position);
            
            position += (unsigned int)strlen(metalnessMapName) + 1;
            
            node.material->settings(material_settings);
            node.material->texture(texture);
            node.length = 0;
            
            return node;
        }
        break;

        case OvObject::Type::MESH:
        case OvObject::Type::SKINNED:
        {
            NodeType node;
            
            node.node = std::make_shared<Mesh>();
            
            bool isSkinned = false;
            //Controllo se la Mesh ha la skin
            if ((OvObject::Type)chunkId == OvObject::Type::SKINNED)
            {
                isSkinned = true;
            }

            //Leggo e setto nome Mesh
            char meshName[FILENAME_MAX];
            strcpy(meshName, data + position);
            
            position += (unsigned int)strlen(meshName) + 1;
            
            dynamic_cast<Mesh*>(node.node.get())->name(meshName);
            
            //Leggo e setto matrice della Mesh
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            
            position += sizeof(glm::mat4);
            
            dynamic_cast<Mesh*>(node.node.get())->setTransformation(matrix);
            
            //Leggo e setto numero di figli
            unsigned int children;
            memcpy(&children, data + position, sizeof(unsigned int));
            
            node.length = children;
            
            position += sizeof(unsigned int);
            
            //Leggo nome del target
            char targetName[FILENAME_MAX];
            strcpy(targetName, data + position);
            
            position += (unsigned int)strlen(targetName) + 1;

            unsigned char subtype;

            //Leggo sottotipo
            memcpy(&subtype, data + position, sizeof(unsigned char));
            char subtypeName[FILENAME_MAX];
            
            //Setto nome del sottotipo
            switch ((OvMesh::Subtype)subtype)
            {
                case OvMesh::Subtype::DEFAULT: strcpy(subtypeName, "standard"); break;
                case OvMesh::Subtype::NORMALMAPPED: strcpy(subtypeName, "normal-mapped"); break;
                case OvMesh::Subtype::TESSELLATED: strcpy(subtypeName, "tessellated"); break;
                default: strcpy(subtypeName, "UNDEFINED");
            }

            position += sizeof(unsigned char);
            
            //Aggiungo nome materiale
            char materialName[FILENAME_MAX];
            strcpy(materialName, data + position);
            
            position += (unsigned int)strlen(materialName) + 1;
            
            //Setto materiale
            if (material[materialName]) {
                dynamic_cast<Mesh*>(node.node.get())->material(material[materialName]);
            }
            
            //Leggo raggio
            float radius;
            memcpy(&radius, data + position, sizeof(float));
            
            position += sizeof(float);
            
            //Leggo bBoxMin
            glm::vec3 bBoxMin;
            memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
            
            position += sizeof(glm::vec3);
            
            //Leggo bBoxMax
            glm::vec3 bBoxMax;
            memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
            
            position += sizeof(glm::vec3);
            
            //Leggo se possiede fisica
            unsigned char hasPhysics;
            memcpy(&hasPhysics, data + position, sizeof(unsigned char));
            
            position += sizeof(unsigned char);

            //Leggo numero di LOD
            unsigned int LODs;
            memcpy(&LODs, data + position, sizeof(unsigned int));
            
            position += sizeof(unsigned int);
            
            dynamic_cast<Mesh*>(node.node.get())->LOD(LODs);
            
            std::vector<unsigned int> verticesPerLOD(LODs);
            LODdata lods;

            for (unsigned int l = 0; l < LODs; l++)
            {
                //face(3,5,2)
                //1(...)2(...)3(..)
                //Leggo numero di vertici
                unsigned int vertices, faces;
                memcpy(&vertices, data + position, sizeof(unsigned int));
                
                position += sizeof(unsigned int);
                
                verticesPerLOD[l] = vertices;
                
                //Leggo numero di facce
                memcpy(&faces, data + position, sizeof(unsigned int));
                
                position += sizeof(unsigned int);
                
                VerticesData vertices_data;
                
                //Leggo tutti i vertici
                glm::vec3 vertex;
                memcpy(&vertex, data + position, sizeof(glm::vec3));

                for (unsigned int c = 0; c < vertices; c++)
                {
                    //Leggo e setto vertice
                    glm::vec3 vertex;
                    memcpy(&vertex, data + position, sizeof(glm::vec3));
                    vertices_data.vertices.push_back(vertex);
                    
                    position += sizeof(glm::vec3);
                    
                    //Leggo e setto normale
                    unsigned int normalData;
                    memcpy(&normalData, data + position, sizeof(unsigned int));

                    glm::vec4 normal = glm::unpackSnorm3x10_1x2(normalData);
                    vertices_data.normal.push_back(normal);

                    position += sizeof(unsigned int);
                    
                    //Leggo e setto coordinate di texturizzazione
                    unsigned int textureData;
                    memcpy(&textureData, data + position, sizeof(unsigned int));

                    glm::vec2 uv = glm::unpackHalf2x16(textureData);
                    vertices_data.uv.push_back(uv);

                    position += sizeof(unsigned int);
                
                    //Leggo tangente
                    unsigned int tangentData;
                    memcpy(&tangentData, data + position, sizeof(unsigned int));
                    glm::vec4 tangent = glm::unpackSnorm3x10_1x2(tangentData);

                    position += sizeof(unsigned int);
                }

                //Leggo tutte le facce
                for (unsigned int c = 0; c < faces; c++)
                {
                    //Leggo i 3 vertici
                    unsigned int face[3];
                    memcpy(face, data + position, sizeof(unsigned int) * 3);
                    
                    position += sizeof(unsigned int) * 3;
                    
                    //Compongo e setto le faccia
                    std::array<unsigned int, 3> face_array;
                    std::copy(std::begin(face), std::end(face), std::begin(face_array));
                    vertices_data.faces.push_back(face_array);
                }

                lods.lod.push_back(vertices_data);
            }
            dynamic_cast<Mesh*>(node.node.get())->VAO(num_vao);
            dynamic_cast<Mesh*>(node.node.get())->vertices(lods);
            num_vao = num_vao + 1;
            node.length = children;
            
            return node;
        }
        break;

        case OvObject::Type::LIGHT:
        {
            NodeType node;
            
            node.node = std::make_shared<Light>();
            
            //Leggo e setto nome luce
            char lightName[FILENAME_MAX];
            strcpy(lightName, data + position);
            node.node->name(lightName);
            
            position += (unsigned int)strlen(lightName) + 1;

            //Leggo e setto matrice luce
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            node.node->setTransformation(matrix);
           
            position += sizeof(glm::mat4);

            //Leggo numero di figli
            unsigned int children;
            memcpy(&children, data + position, sizeof(unsigned int));
            
            position += sizeof(unsigned int);

            //Leggo targetName
            char targetName[FILENAME_MAX];
            strcpy(targetName, data + position);
            
            position += (unsigned int)strlen(targetName) + 1;

            //Leggo sottotipo
            unsigned char subtype;
            memcpy(&subtype, data + position, sizeof(unsigned char));
            
            //char subtypeName[FILENAME_MAX];
            
            LightSettings settings;
            
            //Setto numero luce
            settings.light_number = num_light;
            num_light = num_light + 1;
            
            //Setto tipologia luce
            settings.light_type = (OvLight::Subtype)subtype;
            
            position += sizeof(unsigned char);
        
            //Leggo e setto colore
            glm::vec3 color;
            memcpy(&color, data + position, sizeof(glm::vec3));
            settings.color = color;
            
            position += sizeof(glm::vec3);

            //Leggo e setto raggio
            float radius;
            memcpy(&radius, data + position, sizeof(float));
            settings.radius = radius;
            
            position += sizeof(float);

            //Leggo e setto direzione
            glm::vec3 direction;
            memcpy(&direction, data + position, sizeof(glm::vec3));
            settings.direction = direction;
            
            position += sizeof(glm::vec3);

            //Leggo e setto cutoff
            float cutoff;
            memcpy(&cutoff, data + position, sizeof(float));
            settings.cutoff = cutoff;
            
            position += sizeof(float);

            //Leggo e setto esponente luce spot
            float spotExponent;
            memcpy(&spotExponent, data + position, sizeof(float));
            settings.spot_exponent = spotExponent;
            
            position += sizeof(float);

            //Leggo e setto flag ombre
            unsigned char castShadows;
            memcpy(&castShadows, data + position, sizeof(unsigned char));
            
            position += sizeof(unsigned char);

            //Leggo e setto flag volumetric
            unsigned char isVolumetric;
            memcpy(&isVolumetric, data + position, sizeof(unsigned char));
            
            position += sizeof(unsigned char);
            
            //Carico impostazioni
            dynamic_cast<Light*>(node.node.get())->settings(settings);
            
            node.length = children;
            
            return node;
        }
        break;

        default:
        {
            //Libero data
            delete[] data;
            
            NodeType node;
            node.length = 0;
            
            return node;
        }
    }
}

void LIB_API ObjectLoader::readMaterials(char* buffer, unsigned int& position)
{
    //Leggo chunk
    int chunkId;
    memcpy(&chunkId, buffer + position, sizeof(unsigned int));
    
    position += sizeof(unsigned int);
    
    //Leggo grandezza del chunk
    int chunkSize;
    memcpy(&chunkSize, buffer + position, sizeof(unsigned int));
    
    position += sizeof(unsigned int);
    
    //Leggo e controllo data
    char* data = new char[chunkSize];
    memcpy(data, buffer + position, chunkSize);
    
    NodeType node;
    node = readNode(data, chunkId);
    
    //Libero data
    delete[] data;
    
    unsigned int nrOfChildren = node.length;
    std::shared_ptr<Node> thisNode = node.node;
    
    position += chunkSize;  
    
    memcpy(&chunkId, buffer + position, sizeof(unsigned int));
    
    position += sizeof(unsigned int);
    
    memcpy(&chunkSize, buffer + position, sizeof(unsigned int));
    
    position += sizeof(unsigned int);
    
    //Leggo e controllo data
    data = new char[chunkSize];
    memcpy(data, buffer + position, chunkSize);
    
    node = readNode(data, chunkId);
    
    if (node.material) {
        material[node.material->name()] = node.material;
    }
    
    //Libero data
    delete[] data;
    
    position += chunkSize;
    
    while ((OvObject::Type)chunkId == OvObject::Type::MATERIAL)
    {
        memcpy(&chunkId, buffer + position, sizeof(unsigned int));
        
        position += sizeof(unsigned int);
        
        memcpy(&chunkSize, buffer + position, sizeof(unsigned int));
        
        position += sizeof(unsigned int);
        
        //Leggo e controllo data
        data = new char[chunkSize];
        memcpy(data, buffer + position, chunkSize);
        
        node = readNode(data, chunkId);
        
        if (node.material) {
            material[node.material->name()] = node.material;
        }
        
        //Libero data
        delete[] data;
        
        position += chunkSize;
    }
    
    //torno indietro perchè il while legge 1 oggetto in piu
    position -= sizeof(unsigned int);
    position -= sizeof(unsigned int);
    position -= chunkSize;
}

std::shared_ptr<Node LIB_API> ObjectLoader::recursiveLoad(char* buffer, unsigned int& position)
{
    //Leggo chunk
    int chunkId;
    memcpy(&chunkId, buffer + position, sizeof(unsigned int));
    
    position += sizeof(unsigned int);
    
    //Leggo grandezza chunck
    int chunkSize;
    memcpy(&chunkSize, buffer + position, sizeof(unsigned int));
    
    position += sizeof(unsigned int);
    
    //Leggo e controllo data
    char* data = new char[chunkSize];
    memcpy(data, buffer + position, chunkSize);
    
    NodeType node;
    node = readNode(data, chunkId);
    
    //Libero data
    delete[] data;
    
    int nrOfChildren = node.length;
    
    std::shared_ptr<Node> thisNode = node.node;
    
    position += chunkSize;

    //Leggo figli
    if (nrOfChildren) {
        while (thisNode->getNumberOfChildren() < nrOfChildren)
        {
            std::shared_ptr<Node> childNode = recursiveLoad(buffer, position);
            
            thisNode->addChild(std::shared_ptr<Node>(childNode));
            childNode->parent(thisNode);
        }
    }

    return thisNode;
}
