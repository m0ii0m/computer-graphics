#pragma once

#include <GL/glew.h>

GLuint LoadTexture(const char* filename);
GLuint CreateGridTexture();
GLuint LoadCubemap(const char* faces[6]);
