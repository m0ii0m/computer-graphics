#pragma once

#include <GL/glew.h>
#include "Math.h"
#include <vector>

void ComputeNormals(const GLfloat* input_vertices, int num_triangles, Vertex* output_vertices);
