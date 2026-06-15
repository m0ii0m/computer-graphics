#include "MeshUtils.h"

void ComputeNormals(const GLfloat* input_vertices, int num_triangles, Vertex* output_vertices) {
    int num_vertices = num_triangles * 3;
    std::vector<Vertex> temp_vertices(num_vertices);

    // Calcul des normales des faces
    for (int i = 0; i < num_triangles; ++i) {
        int offset = i * 9;
        Vector3D p0 = { input_vertices[offset+0], input_vertices[offset+1], input_vertices[offset+2] };
        Vector3D p1 = { input_vertices[offset+3], input_vertices[offset+4], input_vertices[offset+5] };
        Vector3D p2 = { input_vertices[offset+6], input_vertices[offset+7], input_vertices[offset+8] };
        
        Vector3D u = Subtract(p1, p0);
        Vector3D v = Subtract(p2, p0);
        Vector3D face_normal = Normalize(CrossProduct(u, v));
        
        temp_vertices[i*3+0] = { p0, face_normal, {0.0f, 0.0f} };
        temp_vertices[i*3+1] = { p1, face_normal, {0.0f, 0.0f} };
        temp_vertices[i*3+2] = { p2, face_normal, {0.0f, 0.0f} };
    }

    // Lissage (moyenne des normales par position spatiale)
    for (int i = 0; i < num_vertices; ++i) {
        Vector3D pos_i = temp_vertices[i].position;
        Vector3D sum_normal = {0.0f, 0.0f, 0.0f};
        
        for (int j = 0; j < num_vertices; ++j) {
            if (ArePositionsEqual(pos_i, temp_vertices[j].position)) {
                sum_normal = Add(sum_normal, temp_vertices[j].normal);
            }
        }
        
        output_vertices[i] = { pos_i, Normalize(sum_normal), {0.0f, 0.0f} };
    }
}
