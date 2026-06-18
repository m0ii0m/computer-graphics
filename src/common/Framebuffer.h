#pragma once

#include <GL/glew.h>

struct Framebuffer {
    GLuint fbo;
    GLuint colorTexture;
    GLuint depthTexture;
    int width;
    int height;
};

Framebuffer CreateFramebuffer(int width, int height);

void DestroyFramebuffer(Framebuffer& fb);

void BindFramebuffer(const Framebuffer& fb);

void UnbindFramebuffer();
