#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/GLShader.h"
#include "common/Math.h"
#include "common/GLTexture.h"
#include "common/MeshUtils.h"
#include "common/OrbitCamera.h"
#include "common/OBJLoader.h"
#include "common/Framebuffer.h"
#include "dragondata.h"
#include <cmath>
#include <vector>
#include <iostream>

const GLfloat g_cube_vertices2[] = {
    -1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f,-1.0f, -1.0f,-1.0f,-1.0f, -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,  1.0f,-1.0f,-1.0f, -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f, -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,  1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,  1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f, -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f, -1.0f, 1.0f, 1.0f
};

// Quad plein écran pour le post-traitement (NDC)
const GLfloat g_quad_vertices[] = {
    // position    // uv
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

// Sol (plan horizontal 20x20)
const GLfloat g_floor_vertices[] = {
    -10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
     10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    -10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
     10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    -10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
};

struct ViewProj {
    Matrix4 viewMatrix;
    Matrix4 projectionMatrix;
};

GLFWwindow* window;
GLShader g_BasicShader;
GLShader g_SkyboxShader;
GLShader g_EnvMapShader;
GLShader g_PostProcessShader;
GLComputeShader g_ProceduralShader;

GLuint cubeVAO, cubeVBO;
Vertex cube_vertices_with_normals[36];

GLuint dragonVAO, dragonVBO, dragonIBO;

// Modèle OBJ (Bunny)
GLuint bunnyVAO, bunnyVBO, bunnyIBO;
std::vector<OBJMesh> bunnyMeshes;
int bunnyIndexCount = 0;
OBJMaterial bunnyMat;

// Sol
GLuint floorVAO, floorVBO;

// Skybox
GLuint skyboxVAO, skyboxVBO;

// Quad post-traitement
GLuint quadVAO, quadVBO;

// Textures
GLuint dragonTex, defaultTex;
GLuint cubemapTex;
GLuint proceduralTex;

// OpenGL 3.x
GLuint uboViewProj;
GLuint linearSampler, nearestSampler, cubemapSampler;

// FBO
Framebuffer fbo;

// Post-traitement
int currentEffect = 0;
const int NUM_EFFECTS = 4;
bool pKeyWasPressed = false;

bool Initialise()
{
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    if (!glfwInit()) return false;

    window = glfwCreateWindow(1280, 720, "Projet OpenGL - REGNIER & HO", NULL, NULL);
    if (!window) { glfwTerminate(); return false; }

    glfwMakeContextCurrent(window);

    // Callbacks souris
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        glfwTerminate();
        return false;
    }

    // Note : on ne fait pas glEnable(GL_FRAMEBUFFER_SRGB) ici
    // car le rendu principal va dans un FBO linéaire (RGBA16F).
    // La correction gamma est faite dans le shader de post-traitement.

    // Shaders
    g_BasicShader.LoadVertexShader("src/cam.vs");
    g_BasicShader.LoadFragmentShader("src/GammaAuto.fs");
    g_BasicShader.Create();

    g_SkyboxShader.LoadVertexShader("src/skybox.vs");
    g_SkyboxShader.LoadFragmentShader("src/skybox.fs");
    g_SkyboxShader.Create();

    g_EnvMapShader.LoadVertexShader("src/envmap.vs");
    g_EnvMapShader.LoadFragmentShader("src/envmap.fs");
    g_EnvMapShader.Create();

    g_PostProcessShader.LoadVertexShader("src/postprocess.vs");
    g_PostProcessShader.LoadFragmentShader("src/postprocess.fs");
    g_PostProcessShader.Create();

    g_ProceduralShader.LoadComputeShader("src/procedural.cs");
    g_ProceduralShader.Create();

    auto basicProgram = g_BasicShader.GetProgram();
    auto skyboxProgram = g_SkyboxShader.GetProgram();
    auto envProgram = g_EnvMapShader.GetProgram();

    // UBO ViewProj
    glGenBuffers(1, &uboViewProj);
    glBindBuffer(GL_UNIFORM_BUFFER, uboViewProj);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ViewProj), NULL, GL_STREAM_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboViewProj);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glUniformBlockBinding(basicProgram, glGetUniformBlockIndex(basicProgram, "ViewProj"), 0);
    glUniformBlockBinding(skyboxProgram, glGetUniformBlockIndex(skyboxProgram, "ViewProj"), 0);
    glUniformBlockBinding(envProgram, glGetUniformBlockIndex(envProgram, "ViewProj"), 0);

    // Samplers
    glGenSamplers(1, &linearSampler);
    glSamplerParameteri(linearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(linearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linearSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(linearSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenSamplers(1, &nearestSampler);
    glSamplerParameteri(nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(nearestSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(nearestSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenSamplers(1, &cubemapSampler);
    glSamplerParameteri(cubemapSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(cubemapSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(cubemapSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(cubemapSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(cubemapSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    int loc_normal = glGetAttribLocation(basicProgram, "a_normal");
    int loc_uv = glGetAttribLocation(basicProgram, "a_uv");

    // Textures
    dragonTex = LoadTexture("assets/dragon.png");

    const char* cubemapFaces[6] = {
        "assets/envmaps/pisa_posx.jpg", "assets/envmaps/pisa_negx.jpg",
        "assets/envmaps/pisa_posy.jpg", "assets/envmaps/pisa_negy.jpg",
        "assets/envmaps/pisa_posz.jpg", "assets/envmaps/pisa_negz.jpg"
    };
    cubemapTex = LoadCubemap(cubemapFaces);

    // Texture blanche par défaut
    glGenTextures(1, &defaultTex);
    glBindTexture(GL_TEXTURE_2D, defaultTex);
    unsigned char white[] = { 255, 255, 255, 255 };
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Texture procédurale via Compute Shader
    const int PROC_SIZE = 512;
    glGenTextures(1, &proceduralTex);
    glBindTexture(GL_TEXTURE_2D, proceduralTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, PROC_SIZE, PROC_SIZE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Dispatch du compute shader
    glUseProgram(g_ProceduralShader.GetProgram());
    glBindImageTexture(0, proceduralTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glUniform1f(glGetUniformLocation(g_ProceduralShader.GetProgram(), "u_time"), 0.0f);
    glDispatchCompute((PROC_SIZE + 15) / 16, (PROC_SIZE + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glUseProgram(0);

    // Cube (Blinn-Phong)
    ComputeNormals(g_cube_vertices2, 12, cube_vertices_with_normals);

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices_with_normals), cube_vertices_with_normals, GL_STATIC_DRAW);

    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    if (loc_normal != -1) {
        glEnableVertexAttribArray(loc_normal);
        glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    }
    if (loc_uv != -1) {
        glEnableVertexAttribArray(loc_uv);
        glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    }
    glBindVertexArray(0);

    // Dragon
    const int DRAGON_STRIDE = 8 * sizeof(GLfloat);
    int env_loc_position = glGetAttribLocation(envProgram, "a_position");
    int env_loc_normal = glGetAttribLocation(envProgram, "a_normal");

    glGenVertexArrays(1, &dragonVAO);
    glBindVertexArray(dragonVAO);
    glGenBuffers(1, &dragonVBO);
    glBindBuffer(GL_ARRAY_BUFFER, dragonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DragonVertices), DragonVertices, GL_STATIC_DRAW);
    glGenBuffers(1, &dragonIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dragonIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(DragonIndices), DragonIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(env_loc_position);
    glVertexAttribPointer(env_loc_position, 3, GL_FLOAT, GL_FALSE, DRAGON_STRIDE, (void*)0);
    if (env_loc_normal != -1) {
        glEnableVertexAttribArray(env_loc_normal);
        glVertexAttribPointer(env_loc_normal, 3, GL_FLOAT, GL_FALSE, DRAGON_STRIDE, (void*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Modèle OBJ (Bunny)
    if (LoadOBJ("assets/models/bunny.obj", bunnyMeshes) && !bunnyMeshes.empty()) {
        OBJMesh& m = bunnyMeshes[0];
        bunnyMat = m.material;
        bunnyIndexCount = (int)m.indices.size();

        glGenVertexArrays(1, &bunnyVAO);
        glBindVertexArray(bunnyVAO);

        glGenBuffers(1, &bunnyVBO);
        glBindBuffer(GL_ARRAY_BUFFER, bunnyVBO);
        glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Vertex), m.vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &bunnyIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(unsigned int), m.indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        if (loc_normal != -1) {
            glEnableVertexAttribArray(loc_normal);
            glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        }
        if (loc_uv != -1) {
            glEnableVertexAttribArray(loc_uv);
            glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // Sol
    glGenVertexArrays(1, &floorVAO);
    glBindVertexArray(floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_floor_vertices), g_floor_vertices, GL_STATIC_DRAW);

    int floorStride = 8 * sizeof(GLfloat);
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, floorStride, (void*)0);
    if (loc_normal != -1) {
        glEnableVertexAttribArray(loc_normal);
        glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, floorStride, (void*)(3 * sizeof(GLfloat)));
    }
    if (loc_uv != -1) {
        glEnableVertexAttribArray(loc_uv);
        glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, floorStride, (void*)(6 * sizeof(GLfloat)));
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Skybox
    int skybox_loc_pos = glGetAttribLocation(skyboxProgram, "a_position");
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_cube_vertices2), g_cube_vertices2, GL_STATIC_DRAW);
    glEnableVertexAttribArray(skybox_loc_pos);
    glVertexAttribPointer(skybox_loc_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Quad post-traitement
    auto ppProgram = g_PostProcessShader.GetProgram();
    int pp_loc_pos = glGetAttribLocation(ppProgram, "a_position");
    int pp_loc_uv = glGetAttribLocation(ppProgram, "a_uv");

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertices), g_quad_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(pp_loc_pos);
    glVertexAttribPointer(pp_loc_pos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    if (pp_loc_uv != -1) {
        glEnableVertexAttribArray(pp_loc_uv);
        glVertexAttribPointer(pp_loc_uv, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // FBO
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    fbo = CreateFramebuffer(w, h);

    return true;
}

void Terminate() {
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glDeleteVertexArrays(1, &dragonVAO);
    glDeleteBuffers(1, &dragonVBO);
    glDeleteBuffers(1, &dragonIBO);

    glDeleteVertexArrays(1, &bunnyVAO);
    glDeleteBuffers(1, &bunnyVBO);
    glDeleteBuffers(1, &bunnyIBO);

    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glDeleteTextures(1, &dragonTex);
    glDeleteTextures(1, &defaultTex);
    glDeleteTextures(1, &cubemapTex);
    glDeleteTextures(1, &proceduralTex);

    glDeleteSamplers(1, &linearSampler);
    glDeleteSamplers(1, &nearestSampler);
    glDeleteSamplers(1, &cubemapSampler);

    glDeleteBuffers(1, &uboViewProj);

    DestroyFramebuffer(fbo);

    g_BasicShader.Destroy();
    g_SkyboxShader.Destroy();
    g_EnvMapShader.Destroy();
    g_PostProcessShader.Destroy();
    g_ProceduralShader.Destroy();

    glfwTerminate();
}

void Render()
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Recréer le FBO si la taille de la fenêtre a changé
    if (width != fbo.width || height != fbo.height) {
        DestroyFramebuffer(fbo);
        fbo = CreateFramebuffer(width, height);
    }

    float time = (float)glfwGetTime();
    float aspect = (float)width / (float)(height);

    // Mise à jour de l'UBO
    ViewProj vp;
    vp.projectionMatrix = Perspective(3.14159265f / 3.0f, aspect, 0.1f, 1000.0f);
    
    Vector3D camTarget = { 0.0f, 0.0f, 0.0f };
    Vector3D camPos;
    camPos.y = cam_radius * std::sin(cam_theta);
    camPos.x = cam_radius * std::cos(cam_theta) * std::cos(cam_phi);
    camPos.z = cam_radius * std::cos(cam_theta) * std::sin(cam_phi);
    Vector3D camUp = { 0.0f, 1.0f, 0.0f };
    
    vp.viewMatrix = LookAt(camPos, camTarget, camUp);

    glBindBuffer(GL_UNIFORM_BUFFER, uboViewProj);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ViewProj), &vp);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // PASSE 1 : Rendu hors écran dans le FBO
    
    BindFramebuffer(fbo);
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);

    // Skybox
    {
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);

        auto skyboxProgram = g_SkyboxShader.GetProgram();
        glUseProgram(skyboxProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
        glBindSampler(0, cubemapSampler);
        glUniform1i(glGetUniformLocation(skyboxProgram, "u_cubemap"), 0);

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glBindSampler(0, 0);

        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
    }

    auto basicProgram = g_BasicShader.GetProgram();
    glUseProgram(basicProgram);

    glUniform3f(glGetUniformLocation(basicProgram, "u_light.position"), 5.0f, 8.0f, -5.0f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.ambientColor"), 0.15f, 0.15f, 0.15f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.diffuseColor"), 0.9f, 0.9f, 0.9f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.specularColor"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(basicProgram, "u_light.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(basicProgram, "u_light.linear"), 0.045f);
    glUniform1f(glGetUniformLocation(basicProgram, "u_light.quadratic"), 0.0075f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_skyColor"), 0.1f, 0.3f, 0.6f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_groundColor"), 0.2f, 0.1f, 0.05f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_viewPos"), camPos.x, camPos.y, camPos.z);

    // Cubemap pour l'illumination indirecte (unit 1)
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
    glBindSampler(1, cubemapSampler);
    glUniform1i(glGetUniformLocation(basicProgram, "u_envmap"), 1);

    // Cube
    {
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.emissiveColor"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.ambientColor"), 0.3f, 0.1f, 0.1f);
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.diffuseColor"), 0.8f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.specularColor"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(basicProgram, "u_material.shininess"), 16.0f);

        Matrix4 rotY = RotateY(time * 1.2f);
        Matrix4 rotX = RotateX(time * 0.8f);
        Matrix4 rot = Multiply(rotX, rotY);
        Matrix4 trans = Translate(-4.0f, 1.0f, 0.0f);
        Matrix4 worldMatrix = Multiply(trans, rot);
        glUniformMatrix4fv(glGetUniformLocation(basicProgram, "u_worldMatrix"), 1, GL_FALSE, worldMatrix.m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, defaultTex);
        glBindSampler(0, nearestSampler);
        glUniform1i(glGetUniformLocation(basicProgram, "u_diffuseTexture"), 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glBindSampler(0, 0);
    }

    // Bunny
    if (bunnyIndexCount > 0) {
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.emissiveColor"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.ambientColor"),
                    bunnyMat.ambientColor.x, bunnyMat.ambientColor.y, bunnyMat.ambientColor.z);
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.diffuseColor"),
                    bunnyMat.diffuseColor.x, bunnyMat.diffuseColor.y, bunnyMat.diffuseColor.z);
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.specularColor"),
                    bunnyMat.specularColor.x, bunnyMat.specularColor.y, bunnyMat.specularColor.z);
        glUniform1f(glGetUniformLocation(basicProgram, "u_material.shininess"), bunnyMat.shininess);

        Matrix4 scale = Scale(30.0f, 30.0f, 30.0f);
        Matrix4 trans = Translate(0.0f, -2.5f, 0.0f);
        Matrix4 rot = RotateY(time * 0.5f);
        Matrix4 worldMatrix = Multiply(trans, Multiply(rot, scale));
        glUniformMatrix4fv(glGetUniformLocation(basicProgram, "u_worldMatrix"), 1, GL_FALSE, worldMatrix.m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, defaultTex);
        glBindSampler(0, nearestSampler);
        glUniform1i(glGetUniformLocation(basicProgram, "u_diffuseTexture"), 0);

        glBindVertexArray(bunnyVAO);
        glDrawElements(GL_TRIANGLES, bunnyIndexCount, GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);
        glBindSampler(0, 0);
    }

    // Sol
    {
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.emissiveColor"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.ambientColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.diffuseColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(basicProgram, "u_material.specularColor"), 0.3f, 0.3f, 0.3f);
        glUniform1f(glGetUniformLocation(basicProgram, "u_material.shininess"), 8.0f);

        Matrix4 trans = Translate(0.0f, -3.0f, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(basicProgram, "u_worldMatrix"), 1, GL_FALSE, trans.m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, proceduralTex);
        glBindSampler(0, linearSampler);
        glUniform1i(glGetUniformLocation(basicProgram, "u_diffuseTexture"), 0);

        glDisable(GL_CULL_FACE);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
        glBindSampler(0, 0);
    }

    // Unbind cubemap de l'unité 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindSampler(1, 0);

    // Dragon
    {
        auto envProgram = g_EnvMapShader.GetProgram();
        glUseProgram(envProgram);

        glUniform3f(glGetUniformLocation(envProgram, "u_viewPos"), camPos.x, camPos.y, camPos.z);

        Matrix4 rot = RotateY(time);
        Matrix4 trans = Translate(4.0f, -1.0f, 0.0f);
        Matrix4 scale = Scale(0.5f, 0.5f, 0.5f);
        Matrix4 worldMatrix = Multiply(trans, Multiply(rot, scale));
        glUniformMatrix4fv(glGetUniformLocation(envProgram, "u_worldMatrix"), 1, GL_FALSE, worldMatrix.m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
        glBindSampler(0, cubemapSampler);
        glUniform1i(glGetUniformLocation(envProgram, "u_cubemap"), 0);

        glBindVertexArray(dragonVAO);
        glDrawElements(GL_TRIANGLES, sizeof(DragonIndices) / sizeof(DragonIndices[0]), GL_UNSIGNED_SHORT, NULL);
        glBindVertexArray(0);
        glBindSampler(0, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // Post-traitement (FBO -> backbuffer)

    UnbindFramebuffer();
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    auto ppProgram = g_PostProcessShader.GetProgram();
    glUseProgram(ppProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo.colorTexture);
    glUniform1i(glGetUniformLocation(ppProgram, "u_screenTexture"), 0);
    glUniform1i(glGetUniformLocation(ppProgram, "u_effect"), currentEffect);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDepthMask(GL_TRUE);
}

int main(void)
{
    if (!Initialise()) return -1;

    while (!glfwWindowShouldClose(window))
    {
        const float keySpeed = 0.03f;
        const float PI = 3.14159265f;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) cam_phi += keySpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) cam_phi -= keySpeed;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) cam_theta += keySpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) cam_theta -= keySpeed;
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) cam_radius -= 0.5f;
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) cam_radius += 0.5f;

        // Toggle post-traitement avec la touche P
        bool pPressed = (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);
        if (pPressed && !pKeyWasPressed) {
            currentEffect = (currentEffect + 1) % NUM_EFFECTS;
            const char* effectNames[] = { "Normal", "Sepia", "Luminance", "Inversion" };
            std::cout << "Effet: " << effectNames[currentEffect] << std::endl;
        }
        pKeyWasPressed = pPressed;

        if (cam_phi > PI) cam_phi -= 2.0f * PI;
        if (cam_phi < -PI) cam_phi += 2.0f * PI;
        if (cam_theta > PI / 2.0f - 0.01f) cam_theta = PI / 2.0f - 0.01f;
        if (cam_theta < -PI / 2.0f + 0.01f) cam_theta = -PI / 2.0f + 0.01f;
        if (cam_radius < 1.0f) cam_radius = 1.0f;
        if (cam_radius > 500.0f) cam_radius = 500.0f;

        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Terminate();
    return 0;
}