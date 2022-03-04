#include "ObjectLoader.h"
LIB_API ObjectLoader::ObjectLoader()
{
    num_light = 0;
    num_texture = 1;
}
LIB_API ObjectLoader::~ObjectLoader()
{
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
    switch ((OvObject::Type)chunkId)
    {

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
        char nodeName[FILENAME_MAX];
        strcpy(nodeName, data + position);
        node.node->name(nodeName);
        position += (unsigned int)strlen(nodeName) + 1;
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
        char materialName[FILENAME_MAX];
        strcpy(materialName, data + position);
        node.material = std::make_shared<Material>();
        position += (unsigned int)strlen(materialName) + 1;
        node.material->name(materialName);
        MaterialSettings material_settings;
        glm::vec3 emission, albedo;
        memcpy(&emission, data + position, sizeof(glm::vec3));
        std::shared_ptr<Texture> texture;
        texture = std::make_shared<Texture>();
        material_settings.emission = emission;
        position += sizeof(glm::vec3);
        memcpy(&albedo, data + position, sizeof(glm::vec3));
        material_settings.ambient = glm::mat3(0.2f) * albedo;
        material_settings.diffuse = glm::mat3(0.6f) * albedo;
        material_settings.specular = glm::mat3(0.4f) * albedo;
        position += sizeof(glm::vec3);
        float roughness;
        memcpy(&roughness, data + position, sizeof(float));
        material_settings.roughness = (1 - glm::sqrt(roughness)) * 128;
        position += sizeof(float);
        float metalness;
        memcpy(&metalness, data + position, sizeof(float));
        position += sizeof(float);
        float alpha;
        memcpy(&alpha, data + position, sizeof(float));
        position += sizeof(float);
        char textureName[FILENAME_MAX];
        strcpy(textureName, data + position);
        texture->name(textureName);
        texture->id(num_texture);
        num_texture = num_texture + 1;
        std::string directory;
        directory = "";
        const size_t last_slash_idx = m_directory.rfind('/');
        if (std::string::npos != last_slash_idx)
        {
            directory = m_directory.substr(0, last_slash_idx);
        }
        directory = directory + "/"+textureName;
        texture->load(directory);
        position += (unsigned int)strlen(textureName) + 1;
        char normalMapName[FILENAME_MAX];
        strcpy(normalMapName, data + position);
        position += (unsigned int)strlen(normalMapName) + 1;
        char heightMapName[FILENAME_MAX];
        strcpy(heightMapName, data + position);
        position += (unsigned int)strlen(heightMapName) + 1;
        char roughnessMapName[FILENAME_MAX];
        strcpy(roughnessMapName, data + position);
        position += (unsigned int)strlen(roughnessMapName) + 1;
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
        if ((OvObject::Type)chunkId == OvObject::Type::SKINNED)
        {
            isSkinned = true;
        }

        char meshName[FILENAME_MAX];
        strcpy(meshName, data + position);
        position += (unsigned int)strlen(meshName) + 1;
        dynamic_cast<Mesh*>(node.node.get())->name(meshName);
        glm::mat4 matrix;
        memcpy(&matrix, data + position, sizeof(glm::mat4));
        position += sizeof(glm::mat4);
        dynamic_cast<Mesh*>(node.node.get())->setTransformation(matrix);
        unsigned int children;
        memcpy(&children, data + position, sizeof(unsigned int));
        node.length = children;
        position += sizeof(unsigned int);
        char targetName[FILENAME_MAX];
        strcpy(targetName, data + position);
        position += (unsigned int)strlen(targetName) + 1;

        unsigned char subtype;
        memcpy(&subtype, data + position, sizeof(unsigned char));
        char subtypeName[FILENAME_MAX];
        switch ((OvMesh::Subtype)subtype)
        {
        case OvMesh::Subtype::DEFAULT: strcpy(subtypeName, "standard"); break;
        case OvMesh::Subtype::NORMALMAPPED: strcpy(subtypeName, "normal-mapped"); break;
        case OvMesh::Subtype::TESSELLATED: strcpy(subtypeName, "tessellated"); break;
        default: strcpy(subtypeName, "UNDEFINED");
        }
        position += sizeof(unsigned char);
        char materialName[FILENAME_MAX];
        strcpy(materialName, data + position);
        position += (unsigned int)strlen(materialName) + 1;
        if (material[materialName]) {
            dynamic_cast<Mesh*>(node.node.get())->material(material[materialName]);
        }
        float radius;
        memcpy(&radius, data + position, sizeof(float));
        position += sizeof(float);
        glm::vec3 bBoxMin;
        memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
        position += sizeof(glm::vec3);
        glm::vec3 bBoxMax;
        memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
        position += sizeof(glm::vec3);
        unsigned char hasPhysics;
        memcpy(&hasPhysics, data + position, sizeof(unsigned char));
        position += sizeof(unsigned char);

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
            unsigned int vertices, faces;
            memcpy(&vertices, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);
            verticesPerLOD[l] = vertices;
            memcpy(&faces, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);
            VerticesData vertices_data;
            glm::vec3 vertex;
            memcpy(&vertex, data + position, sizeof(glm::vec3));

            for (unsigned int c = 0; c < vertices; c++)
            {
                glm::vec3 vertex;
                memcpy(&vertex, data + position, sizeof(glm::vec3));
                vertices_data.vertices.push_back(vertex);
                position += sizeof(glm::vec3);
                unsigned int normalData;
                memcpy(&normalData, data + position, sizeof(unsigned int));

                glm::vec4 normal = glm::unpackSnorm3x10_1x2(normalData);
                vertices_data.normal.push_back(normal);

                position += sizeof(unsigned int);
                unsigned int textureData;
                memcpy(&textureData, data + position, sizeof(unsigned int));

                glm::vec2 uv = glm::unpackHalf2x16(textureData);
                vertices_data.uv.push_back(uv);

                position += sizeof(unsigned int);
                
                unsigned int tangentData;
                memcpy(&tangentData, data + position, sizeof(unsigned int));
                glm::vec4 tangent = glm::unpackSnorm3x10_1x2(tangentData);

                position += sizeof(unsigned int);
            }
            for (unsigned int c = 0; c < faces; c++)
            {
                unsigned int face[3];
                memcpy(face, data + position, sizeof(unsigned int) * 3);
                position += sizeof(unsigned int) * 3;
                std::array<unsigned int, 3> face_array;
                std::copy(std::begin(face), std::end(face), std::begin(face_array));
                vertices_data.faces.push_back(face_array);
            }
            lods.lod.push_back(vertices_data);
        }
        dynamic_cast<Mesh*>(node.node.get())->vertices(lods);


        node.length = children;
        return node;
    }
    break;



    case OvObject::Type::LIGHT:
    {
        NodeType node;
        node.node = std::make_shared<Light>();
        char lightName[FILENAME_MAX];
        strcpy(lightName, data + position);
        node.node->name(lightName);
        position += (unsigned int)strlen(lightName) + 1;

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

        unsigned char subtype;
        memcpy(&subtype, data + position, sizeof(unsigned char));
        char subtypeName[FILENAME_MAX];
        LightSettings settings;
        settings.light_number = num_light;
        num_light = num_light + 1;
        settings.light_type = (OvLight::Subtype)subtype;
        position += sizeof(unsigned char);
        
        glm::vec3 color;
        memcpy(&color, data + position, sizeof(glm::vec3));
        settings.color = color;
        position += sizeof(glm::vec3);

        float radius;
        memcpy(&radius, data + position, sizeof(float));
        settings.radius = radius;
        position += sizeof(float);

        glm::vec3 direction;
        memcpy(&direction, data + position, sizeof(glm::vec3));
        settings.direction = direction;
        position += sizeof(glm::vec3);

        float cutoff;
        memcpy(&cutoff, data + position, sizeof(float));
        settings.cutoff = cutoff;
        position += sizeof(float);

        float spotExponent;
        memcpy(&spotExponent, data + position, sizeof(float));
        settings.spot_exponent = spotExponent;
        position += sizeof(float);

        unsigned char castShadows;
        memcpy(&castShadows, data + position, sizeof(unsigned char));
        position += sizeof(unsigned char);

        unsigned char isVolumetric;
        memcpy(&isVolumetric, data + position, sizeof(unsigned char));
        position += sizeof(unsigned char);
        dynamic_cast<Light*>(node.node.get())->settings(settings);
        node.length = children;
        return node;
    }
    break;


    
    default: 
        delete[] data;
        NodeType node;
        node.length = 0;
        return node;
    }
}

void LIB_API ObjectLoader::readMaterials(char* buffer, unsigned int& position)
{
    int chunkId;
    memcpy(&chunkId, buffer + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
    int chunkSize;
    memcpy(&chunkSize, buffer + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
    char* data = new char[chunkSize];
    memcpy(data, buffer + position, chunkSize);
    NodeType node;
    node = readNode(data, chunkId);
    delete[] data;
    unsigned int nrOfChildren = node.length;
    std::shared_ptr<Node> thisNode = node.node;
    position += chunkSize;  
    memcpy(&chunkId, buffer + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
    memcpy(&chunkSize, buffer + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
    data = new char[chunkSize];
    memcpy(data, buffer + position, chunkSize);
    node = readNode(data, chunkId);
    if (node.material) {
        material[node.material->name()] = node.material;
    }
    delete[] data;
    position += chunkSize;
    while ((OvObject::Type)chunkId == OvObject::Type::MATERIAL)
    {
        memcpy(&chunkId, buffer + position, sizeof(unsigned int));
        position += sizeof(unsigned int);
        memcpy(&chunkSize, buffer + position, sizeof(unsigned int));
        position += sizeof(unsigned int);
        data = new char[chunkSize];
        memcpy(data, buffer + position, chunkSize);
        node = readNode(data, chunkId);
        if (node.material) {
            material[node.material->name()] = node.material;
        }
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
    int chunkId;
    memcpy(&chunkId, buffer + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
    int chunkSize;
    memcpy(&chunkSize, buffer + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
    char* data = new char[chunkSize];
    memcpy(data, buffer + position, chunkSize);
    NodeType node;
    node = readNode(data, chunkId);
    delete[] data;
    int nrOfChildren = node.length;
    std::shared_ptr<Node> thisNode = node.node;
    position += chunkSize;

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
