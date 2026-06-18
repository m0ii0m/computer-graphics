#include "Framebuffer.h"
#include <iostream>
#include <cassert>

Framebuffer CreateFramebuffer(int width, int height) {
    Framebuffer fb;
    fb.width = width;
    fb.height = height;

    // Création du FBO
    glGenFramebuffers(1, &fb.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);

    // Color attachment (RGBA16F pour travailler en espace linéaire HDR)
    glGenTextures(1, &fb.colorTexture);
    glBindTexture(GL_TEXTURE_2D, fb.colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb.colorTexture, 0);

    // Depth attachment (24 bits)
    glGenTextures(1, &fb.depthTexture);
    glBindTexture(GL_TEXTURE_2D, fb.depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb.depthTexture, 0);

    // Vérification FBO
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Erreur FBO: " << status << std::endl;
    }
    assert(status == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return fb;
}

void DestroyFramebuffer(Framebuffer& fb) {
    glDeleteTextures(1, &fb.colorTexture);
    glDeleteTextures(1, &fb.depthTexture);
    glDeleteFramebuffers(1, &fb.fbo);
    fb.fbo = 0;
    fb.colorTexture = 0;
    fb.depthTexture = 0;
}

void BindFramebuffer(const Framebuffer& fb) {
    glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
    glViewport(0, 0, fb.width, fb.height);
}

void UnbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
