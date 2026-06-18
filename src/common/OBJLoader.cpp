#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "OBJLoader.h"

#include <iostream>
#include <map>

bool LoadOBJ(const char* filepath, std::vector<OBJMesh>& meshes) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Extraire le répertoire du fichier pour localiser le .mtl
    std::string path(filepath);
    std::string baseDir = "";
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        baseDir = path.substr(0, lastSlash + 1);
    }

    // Chargement avec triangulation forcée
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                filepath, baseDir.c_str(), true);

    if (!warn.empty()) {
        std::cout << "TinyOBJ Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "TinyOBJ Error: " << err << std::endl;
    }
    if (!ret) {
        return false;
    }

    // Parcourir chaque shape (groupe de faces)
    for (size_t s = 0; s < shapes.size(); s++) {
        OBJMesh mesh;

        // Récupérer le matériau de la première face (on suppose un matériau par shape)
        int matID = -1;
        if (!shapes[s].mesh.material_ids.empty()) {
            matID = shapes[s].mesh.material_ids[0];
        }

        if (matID >= 0 && matID < (int)materials.size()) {
            const tinyobj::material_t& mat = materials[matID];
            mesh.material.ambientColor = { mat.ambient[0], mat.ambient[1], mat.ambient[2] };
            mesh.material.diffuseColor = { mat.diffuse[0], mat.diffuse[1], mat.diffuse[2] };
            mesh.material.specularColor = { mat.specular[0], mat.specular[1], mat.specular[2] };
            mesh.material.shininess = mat.shininess;

            if (!mat.diffuse_texname.empty()) {
                mesh.material.diffuseTexturePath = baseDir + mat.diffuse_texname;
            }
        } else {
            // Matériau par défaut (gris)
            mesh.material.ambientColor = { 0.2f, 0.2f, 0.2f };
            mesh.material.diffuseColor = { 0.8f, 0.8f, 0.8f };
            mesh.material.specularColor = { 1.0f, 1.0f, 1.0f };
            mesh.material.shininess = 32.0f;
        }

        // Reconstruction des vertices
        std::map<std::string, unsigned int> uniqueVertices;
        size_t indexOffset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = shapes[s].mesh.num_face_vertices[f]; // devrait être 3 (triangulé)

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];

                Vertex vertex;

                // Position (toujours présente)
                vertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
                vertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
                vertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];

                // Normale (si présente)
                if (idx.normal_index >= 0 && !attrib.normals.empty()) {
                    vertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
                    vertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
                    vertex.normal.z = attrib.normals[3 * idx.normal_index + 2];
                } else {
                    vertex.normal = { 0.0f, 0.0f, 0.0f };
                }

                // Coordonnées de texture (si présentes)
                if (idx.texcoord_index >= 0 && !attrib.texcoords.empty()) {
                    vertex.uv.u = attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.uv.v = attrib.texcoords[2 * idx.texcoord_index + 1];
                } else {
                    vertex.uv = { 0.0f, 0.0f };
                }

                // Clé unique pour dupliquer les vertices
                std::string key = std::to_string(idx.vertex_index) + "/" +
                                  std::to_string(idx.normal_index) + "/" +
                                  std::to_string(idx.texcoord_index);

                if (uniqueVertices.count(key) == 0) {
                    uniqueVertices[key] = (unsigned int)mesh.vertices.size();
                    mesh.vertices.push_back(vertex);
                }

                mesh.indices.push_back(uniqueVertices[key]);
            }

            indexOffset += fv;
        }

        // Si les normales n'étaient pas dans le fichier, les recalculer par face
        if (attrib.normals.empty()) {
            // Initialiser toutes les normales à zéro
            for (size_t i = 0; i < mesh.vertices.size(); i++) {
                mesh.vertices[i].normal = { 0.0f, 0.0f, 0.0f };
            }

            // Accumuler les normales de face sur chaque vertex
            for (size_t i = 0; i < mesh.indices.size(); i += 3) {
                unsigned int i0 = mesh.indices[i + 0];
                unsigned int i1 = mesh.indices[i + 1];
                unsigned int i2 = mesh.indices[i + 2];

                Vector3D p0 = mesh.vertices[i0].position;
                Vector3D p1 = mesh.vertices[i1].position;
                Vector3D p2 = mesh.vertices[i2].position;

                Vector3D edge1 = Subtract(p1, p0);
                Vector3D edge2 = Subtract(p2, p0);
                Vector3D faceNormal = CrossProduct(edge1, edge2);

                mesh.vertices[i0].normal = Add(mesh.vertices[i0].normal, faceNormal);
                mesh.vertices[i1].normal = Add(mesh.vertices[i1].normal, faceNormal);
                mesh.vertices[i2].normal = Add(mesh.vertices[i2].normal, faceNormal);
            }

            // Normaliser
            for (size_t i = 0; i < mesh.vertices.size(); i++) {
                mesh.vertices[i].normal = Normalize(mesh.vertices[i].normal);
            }
        }

        meshes.push_back(mesh);
    }

    std::cout << "OBJ charge: " << filepath
              << " (" << shapes.size() << " shapes, "
              << materials.size() << " materiaux)" << std::endl;

    return true;
}
