#pragma once

#include <GL/glew.h>
#include "GLMath.h"

void ComputeNormals(const GLfloat* input_vertices, int num_triangles, Vertex* output_vertices);
