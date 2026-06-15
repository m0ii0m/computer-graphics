#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/GLShader.h"
#include "common/GLMath.h"
#include "common/GLTexture.h"
#include "common/OrbitCamera.h"
#include "common/MeshUtils.h"
#include "dragondata.h"

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

GLFWwindow* window;
GLShader g_BasicShader;

GLuint cubeVAO, cubeVBO;
Vertex cube_vertices_with_normals[36];

GLuint dragonVAO, dragonVBO, dragonIBO;
GLuint dragonTex, gridTex, defaultTex;

OrbitCamera g_Camera;

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
    g_Camera.RegisterCallbacks(window);

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

    // Shader
    g_BasicShader.LoadVertexShader("src/cam.vs");
    g_BasicShader.LoadFragmentShader("src/GammaAuto.fs");
    g_BasicShader.Create();

    auto basicProgram = g_BasicShader.GetProgram();
    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    int loc_normal = glGetAttribLocation(basicProgram, "a_normal");
    int loc_uv = glGetAttribLocation(basicProgram, "a_uv");

    // Chargement des textures
    dragonTex = LoadTexture("assets/dragon.png");
    gridTex = CreateGridTexture();

    // Texture blanche par défaut
    glGenTextures(1, &defaultTex);
    glBindTexture(GL_TEXTURE_2D, defaultTex);
    unsigned char white[] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Cube
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
    // DragonVertices : format X,Y,Z, NX, NY, NZ, U, V = 8 floats par vertex
    const int DRAGON_STRIDE = 8 * sizeof(GLfloat);

    glGenVertexArrays(1, &dragonVAO);
    glBindVertexArray(dragonVAO);

    glGenBuffers(1, &dragonVBO);
    glBindBuffer(GL_ARRAY_BUFFER, dragonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DragonVertices), DragonVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &dragonIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dragonIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(DragonIndices), DragonIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, DRAGON_STRIDE, (void*)0);
    if (loc_normal != -1) {
        glEnableVertexAttribArray(loc_normal);
        glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, DRAGON_STRIDE, (void*)(3 * sizeof(GLfloat)));
    }
    if (loc_uv != -1) {
        glEnableVertexAttribArray(loc_uv);
        glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, DRAGON_STRIDE, (void*)(6 * sizeof(GLfloat)));
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

void Terminate() {
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glDeleteVertexArrays(1, &dragonVAO);
    glDeleteBuffers(1, &dragonVBO);
    glDeleteBuffers(1, &dragonIBO);

    glDeleteTextures(1, &dragonTex);
    glDeleteTextures(1, &gridTex);
    glDeleteTextures(1, &defaultTex);

    g_BasicShader.Destroy();

    glfwTerminate();
}

void Render()
{
    // etape a. A vous de recuperer/passer les variables width/height
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // etape b. Notez que glClearColor est un etat, donc persistant
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activer le depth test et le culling pour le rendu 3D
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // etape c. on specifie le shader program a utiliser
    auto basicProgram = g_BasicShader.GetProgram();
    glUseProgram(basicProgram);

    // Configuration de la lumière
    // Position de la lumière au milieu de la scène
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.position"), 0.0f, 2.0f, -9.0f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.ambientColor"), 0.1f, 0.1f, 0.1f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.diffuseColor"), 0.9f, 0.9f, 0.9f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_light.specularColor"), 1.0f, 1.0f, 1.0f);

    // Coefficients d'atténuation (Kc, Kl, Kq)
    glUniform1f(glGetUniformLocation(basicProgram, "u_light.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(basicProgram, "u_light.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(basicProgram, "u_light.quadratic"), 0.032f);

    // Propriétés du matériau par défaut
    glUniform3f(glGetUniformLocation(basicProgram, "u_material.emissiveColor"), 0.0f, 0.0f, 0.0f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_material.ambientColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_material.diffuseColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_material.specularColor"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(basicProgram, "u_material.shininess"), 32.0f);

    // Ambiance Hémisphérique (TP7)
    glUniform3f(glGetUniformLocation(basicProgram, "u_skyColor"), 0.1f, 0.3f, 0.6f);
    glUniform3f(glGetUniformLocation(basicProgram, "u_groundColor"), 0.2f, 0.1f, 0.05f);

    // etape d.
    // glVertexAttribPointer(index, taille, type, normalisation, écart, adresse)
    // (Déplacé dans Initialise avec le VAO)

    // etape e.

    // etape f. dessin de triangles dont la definition provient d’un tableau
    // le rendu s’effectue ici en prenant 3 sommets a partir du debut du tableau (0)

    int loc_worldMatrix = glGetUniformLocation(basicProgram, "u_worldMatrix");
    int loc_viewMatrix = glGetUniformLocation(basicProgram, "u_viewMatrix");
    int loc_projectionMatrix = glGetUniformLocation(basicProgram, "u_projectionMatrix");

    float time = glfwGetTime();
    float aspect = (float)width / (float)(height);
    Matrix4 proj = Perspective(3.14159265f / 3.0f, aspect, 0.1f, 1000.0f);

    Matrix4 view = g_Camera.GetViewMatrix();
    Vector3D camPos = g_Camera.GetPosition();

    // Projection et View (communes à tous les objets)
    glUniformMatrix4fv(loc_projectionMatrix, 1, GL_FALSE, proj.m);
    glUniformMatrix4fv(loc_viewMatrix, 1, GL_FALSE, view.m);

    // Position de la caméra pour le calcul spéculaire
    glUniform3f(glGetUniformLocation(basicProgram, "u_viewPos"), camPos.x, camPos.y, camPos.z);

    // Rendu du Cube
    {
        // World Matrix = Translation * Rotation (Eq 1.1)
        Matrix4 rotY = RotateY(time * 1.2f);
        Matrix4 rotX = RotateX(time * 0.8f);
        Matrix4 rot = Multiply(rotX, rotY);
        Matrix4 trans = Translate(-2.5f, 0.0f, 0.0f);
        Matrix4 worldMatrix = Multiply(trans, rot);

        glUniformMatrix4fv(loc_worldMatrix, 1, GL_FALSE, worldMatrix.m);

        // Liaison de la texture par défaut
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, defaultTex);
        glUniform1i(glGetUniformLocation(basicProgram, "u_diffuseTexture"), 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Rendu du Dragon
    {
        // World Matrix = Translation * Rotation * Scale (Eq 1.1)
        Matrix4 rot = RotateY(time);
        Matrix4 trans = Translate(2.0f, -1.0f, 0.0f);
        Matrix4 scale = Scale(0.5f, 0.5f, 0.5f);
        Matrix4 worldMatrix = Multiply(trans, Multiply(rot, scale));

        glUniformMatrix4fv(loc_worldMatrix, 1, GL_FALSE, worldMatrix.m);

        // Liaison de la texture du dragon
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dragonTex);
        glUniform1i(glGetUniformLocation(basicProgram, "u_diffuseTexture"), 0);

        glBindVertexArray(dragonVAO);
        glDrawElements(GL_TRIANGLES, sizeof(DragonIndices) / sizeof(DragonIndices[0]), GL_UNSIGNED_SHORT, NULL);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

int main(void)
{
    if (!Initialise()) return -1;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Clavier / Update camera
        g_Camera.Update(window);

        /* Render here */
        Render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    Terminate();
    return 0;
}