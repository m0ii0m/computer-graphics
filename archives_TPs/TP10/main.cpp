#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/GLShader.h"
#include "common/Math.h"
#include "common/GLTexture.h"
#include "common/MeshUtils.h"
#include "common/OrbitCamera.h"
#include "dragondata.h"
#include <cmath>

const GLfloat g_cube_vertices2[] = {
    -1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, // Left Side 
    -1.0f,-1.0f,-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f, // Left Side
     1.0f, 1.0f,-1.0f, -1.0f,-1.0f,-1.0f, -1.0f, 1.0f,-1.0f, // Back Side
     1.0f, 1.0f,-1.0f,  1.0f,-1.0f,-1.0f, -1.0f,-1.0f,-1.0f, // Back Side
     1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f, // Bottom Side
     1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f, // Bottom Side
    -1.0f, 1.0f, 1.0f, -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f, // Front Side
    -1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f, // Front Side 
     1.0f, 1.0f, 1.0f,  1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f, // Right Side 
     1.0f,-1.0f,-1.0f,  1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f, // Right Side
     1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f, -1.0f, 1.0f,-1.0f, // Top Side
     1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f, -1.0f, 1.0f, 1.0f  // Top Side
};

struct ViewProj {
    Matrix4 viewMatrix;
    Matrix4 projectionMatrix;
};

GLFWwindow* window;
GLShader g_BasicShader;
GLShader g_SkyboxShader;
GLShader g_EnvMapShader;

GLuint cubeVAO, cubeVBO;
Vertex cube_vertices_with_normals[36];

GLuint dragonVAO, dragonVBO, dragonIBO;
GLuint dragonTex, gridTex, defaultTex;

GLuint skyboxVAO, skyboxVBO;
GLuint cubemapTex;

// OpenGL 3.x variables
GLuint uboViewProj;
GLuint linearSampler;
GLuint nearestSampler;
GLuint cubemapSampler;

bool Initialise()
{
    /* Initialize the library */
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Callbacks souris
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        glfwTerminate();
        return false;
    }

    // Activation de la correction Gamma automatique sur le Framebuffer
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Shader Blinn-Phong (avec Geometry Shader wireframe)
    g_BasicShader.LoadVertexShader("src/cam.vs");
    g_BasicShader.LoadGeometryShader("src/wireframe.gs");
    g_BasicShader.LoadFragmentShader("src/GammaAuto.fs");
    g_BasicShader.Create();

    // Shader Skybox
    g_SkyboxShader.LoadVertexShader("src/skybox.vs");
    g_SkyboxShader.LoadFragmentShader("src/skybox.fs");
    g_SkyboxShader.Create();

    // Shader Environment Map
    g_EnvMapShader.LoadVertexShader("src/envmap.vs");
    g_EnvMapShader.LoadFragmentShader("src/envmap.fs");
    g_EnvMapShader.Create();

    auto basicProgram = g_BasicShader.GetProgram();
    auto skyboxProgram = g_SkyboxShader.GetProgram();
    auto envProgram = g_EnvMapShader.GetProgram();

    // Configuration UBO
    glGenBuffers(1, &uboViewProj);
    glBindBuffer(GL_UNIFORM_BUFFER, uboViewProj);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ViewProj), NULL, GL_STREAM_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboViewProj); // Point d'ancrage 0
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Lier les blocs ViewProj des shaders au point d'ancrage 0
    glUniformBlockBinding(basicProgram, glGetUniformBlockIndex(basicProgram, "ViewProj"), 0);
    glUniformBlockBinding(skyboxProgram, glGetUniformBlockIndex(skyboxProgram, "ViewProj"), 0);
    glUniformBlockBinding(envProgram, glGetUniformBlockIndex(envProgram, "ViewProj"), 0);

    // Configuration Samplers
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

    // Chargement des textures (immuables)
    dragonTex = LoadTexture("assets/dragon.png");
    gridTex = CreateGridTexture();

    // Chargement de la Cubemap (+x, -x, +y, -y, +z, -z)
    const char* cubemapFaces[6] = {
        "assets/envmaps/pisa_posx.jpg",
        "assets/envmaps/pisa_negx.jpg",
        "assets/envmaps/pisa_posy.jpg",
        "assets/envmaps/pisa_negy.jpg",
        "assets/envmaps/pisa_posz.jpg",
        "assets/envmaps/pisa_negz.jpg"
    };
    cubemapTex = LoadCubemap(cubemapFaces);

    // Texture blanche par défaut (immuable)
    glGenTextures(1, &defaultTex);
    glBindTexture(GL_TEXTURE_2D, defaultTex);
    unsigned char white[] = { 255, 255, 255, 255 };
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glBindTexture(GL_TEXTURE_2D, 0);

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

    glGenVertexArrays(1, &dragonVAO);
    glBindVertexArray(dragonVAO);

    glGenBuffers(1, &dragonVBO);
    glBindBuffer(GL_ARRAY_BUFFER, dragonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DragonVertices), DragonVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &dragonIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dragonIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(DragonIndices), DragonIndices, GL_STATIC_DRAW);

    int env_loc_position = glGetAttribLocation(envProgram, "a_position");
    int env_loc_normal = glGetAttribLocation(envProgram, "a_normal");

    glEnableVertexAttribArray(env_loc_position);
    glVertexAttribPointer(env_loc_position, 3, GL_FLOAT, GL_FALSE, DRAGON_STRIDE, (void*)0);
    if (env_loc_normal != -1) {
        glEnableVertexAttribArray(env_loc_normal);
        glVertexAttribPointer(env_loc_normal, 3, GL_FLOAT, GL_FALSE, DRAGON_STRIDE, (void*)(3 * sizeof(GLfloat)));
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Skybox VAO
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

    return true;
}

void Terminate() {
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glDeleteVertexArrays(1, &dragonVAO);
    glDeleteBuffers(1, &dragonVBO);
    glDeleteBuffers(1, &dragonIBO);

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glDeleteTextures(1, &dragonTex);
    glDeleteTextures(1, &gridTex);
    glDeleteTextures(1, &defaultTex);
    glDeleteTextures(1, &cubemapTex);

    glDeleteSamplers(1, &linearSampler);
    glDeleteSamplers(1, &nearestSampler);
    glDeleteSamplers(1, &cubemapSampler);

    glDeleteBuffers(1, &uboViewProj);

    g_BasicShader.Destroy();
    g_SkyboxShader.Destroy();
    g_EnvMapShader.Destroy();

    glfwTerminate();
}

void Render()
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    float time = glfwGetTime();
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

    // --- Rendu de la Skybox ---
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
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
    }

    // --- Rendu des objets Blinn-Phong (Cube) ---
    auto basicProgram = g_BasicShader.GetProgram();
    glUseProgram(basicProgram);

    glUniform3f(glGetUniformLocation(basicProgram, "u_light.position"), 0.0f, 2.0f, -9.0f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.ambientColor"), 0.1f, 0.1f, 0.1f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.diffuseColor"), 0.9f, 0.9f, 0.9f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.specularColor"), 1.0f, 1.0f, 1.0f);

    glUniform1f(glGetUniformLocation(basicProgram, "u_light.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(basicProgram, "u_light.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(basicProgram, "u_light.quadratic"), 0.032f);

    glUniform3f(glGetUniformLocation(basicProgram, "u_material.emissiveColor"), 0.0f, 0.0f, 0.0f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_material.ambientColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_material.diffuseColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_material.specularColor"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(basicProgram, "u_material.shininess"), 32.0f);

    glUniform3f(glGetUniformLocation(basicProgram, "u_skyColor"), 0.1f, 0.3f, 0.6f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_groundColor"), 0.2f, 0.1f, 0.05f);

    glUniform3f(glGetUniformLocation(basicProgram, "u_viewPos"), camPos.x, camPos.y, camPos.z);

    {
        Matrix4 rotY = RotateY(time * 1.2f);
        Matrix4 rotX = RotateX(time * 0.8f);
        Matrix4 rot = Multiply(rotX, rotY);
        Matrix4 trans = Translate(-2.5f, 0.0f, 0.0f);
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
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // --- Rendu du Dragon avec Environment Mapping ---
    {
        auto envProgram = g_EnvMapShader.GetProgram();
        glUseProgram(envProgram);

        glUniform3f(glGetUniformLocation(envProgram, "u_viewPos"), camPos.x, camPos.y, camPos.z);

        Matrix4 rot = RotateY(time);
        Matrix4 trans = Translate(2.0f, -1.0f, 0.0f);
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