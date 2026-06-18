#pragma once

#include <GL/glew.h>
#include "Math.h"
#include <vector>
#include <string>

struct OBJMaterial {
    Vector3D ambientColor;
    Vector3D diffuseColor;
    Vector3D specularColor;
    float shininess;
    std::string diffuseTexturePath;
};

struct OBJMesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    OBJMaterial material;
};

bool LoadOBJ(const char* filepath, std::vector<OBJMesh>& meshes);
