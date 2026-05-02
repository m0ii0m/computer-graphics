#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/GLShader.h"
#include "dragondata.h"
#include <cmath>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "common/stb_image.h"

struct Vector2D {
    float u, v;
};

struct Vector3D {
    float x, y, z;
};

struct Vertex {
    Vector3D position;
    Vector3D normal;
    Vector2D uv;
};

Vector3D Subtract(const Vector3D& a, const Vector3D& b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

Vector3D Add(const Vector3D& a, const Vector3D& b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

bool ArePositionsEqual(const Vector3D& a, const Vector3D& b) {
    const float epsilon = 0.001f;
    return std::abs(a.x - b.x) < epsilon &&
           std::abs(a.y - b.y) < epsilon &&
           std::abs(a.z - b.z) < epsilon;
}

Vector3D CrossProduct(const Vector3D& a, const Vector3D& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vector3D Normalize(const Vector3D& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length > 0.0f) {
        return { v.x / length, v.y / length, v.z / length };
    }
    return v;
}

struct Matrix4 {
    float m[16];

    Matrix4() {
        for (int i = 0; i < 16; ++i) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f; // Identity
    }
};

Matrix4 Translate(float x, float y, float z) {
    Matrix4 mat;
    mat.m[12] = x;
    mat.m[13] = y;
    mat.m[14] = z;
    return mat;
}

Matrix4 RotateX(float angle) {
    Matrix4 mat;
    float c = std::cos(angle);
    float s = std::sin(angle);
    mat.m[5] = c;
    mat.m[6] = s;
    mat.m[9] = -s;
    mat.m[10] = c;
    return mat;
}

Matrix4 RotateY(float angle) {
    Matrix4 mat;
    float c = std::cos(angle);
    float s = std::sin(angle);
    mat.m[0] = c;
    mat.m[2] = -s;
    mat.m[8] = s;
    mat.m[10] = c;
    return mat;
}

Matrix4 RotateZ(float angle) {
    Matrix4 mat;
    float c = std::cos(angle);
    float s = std::sin(angle);
    mat.m[0] = c;
    mat.m[1] = s;
    mat.m[4] = -s;
    mat.m[5] = c;
    return mat;
}

Matrix4 Scale(float sx, float sy, float sz) {
    Matrix4 mat;
    mat.m[0] = sx;
    mat.m[5] = sy;
    mat.m[10] = sz;
    return mat;
}

Matrix4 Multiply(const Matrix4& a, const Matrix4& b) {
    Matrix4 res;
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            res.m[i * 4 + j] = a.m[0 * 4 + j] * b.m[i * 4 + 0] +
                               a.m[1 * 4 + j] * b.m[i * 4 + 1] +
                               a.m[2 * 4 + j] * b.m[i * 4 + 2] +
                               a.m[3 * 4 + j] * b.m[i * 4 + 3];
        }
    }
    return res;
}

Matrix4 Perspective(float fovY, float aspect, float near, float far) {
    Matrix4 mat;
    for (int i = 0; i < 16; ++i) mat.m[i] = 0.0f;
    float cotan = 1.0f / std::tan(fovY / 2.0f);
    mat.m[0] = cotan / aspect;
    mat.m[5] = cotan;
    mat.m[10] = -(far + near) / (far - near);
    mat.m[11] = -1.0f;
    mat.m[14] = -(2.0f * far * near) / (far - near);
    return mat;
}

const GLfloat g_cube_vertices2[] = {
    -1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, // Left Side 
    -1.0f,-1.0f,-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f, // Left Side
     1.0f, 1.0f,-1.0f, -1.0f,-1.0f,-1.0f, -1.0f, 1.0f,-1.0f, // Back Side
     1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f, // Bottom Side
     1.0f, 1.0f,-1.0f,  1.0f,-1.0f,-1.0f, -1.0f,-1.0f,-1.0f, // Back Side
     1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f, // Bottom Side
    -1.0f, 1.0f, 1.0f, -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f, // Front Side
     1.0f, 1.0f, 1.0f,  1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f, // Right Side 
     1.0f,-1.0f,-1.0f,  1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f, // Right Side
     1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f, -1.0f, 1.0f,-1.0f, // Top Side
     1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f, -1.0f, 1.0f, 1.0f, // Top Side
     1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f  // Front Side 
};

GLFWwindow* window;
GLShader g_BasicShader;

GLuint cubeVAO, cubeVBO;
Vertex cube_vertices_with_normals[36];

GLuint dragonVAO, dragonVBO, dragonIBO;
GLuint dragonTex, defaultTex;

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

GLuint LoadTexture(const char* filename) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

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

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        glfwTerminate();
        return false;
    }

    // Shader
    g_BasicShader.LoadVertexShader("BlinnPhong.vs");
    g_BasicShader.LoadFragmentShader("BlinnPhong.fs");
    g_BasicShader.Create();

    auto basicProgram = g_BasicShader.GetProgram();
    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    int loc_normal = glGetAttribLocation(basicProgram, "a_normal");
    int loc_uv = glGetAttribLocation(basicProgram, "a_uv");

    // Chargement de la texture
    dragonTex = LoadTexture("dragon.png");

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

    // Position de la caméra (en 0,0,0 car on déplace les objets)
    glUniform3f(glGetUniformLocation(basicProgram, "u_viewPos"), 0.0f, 0.0f, 0.0f);

    // etape d.
    // glVertexAttribPointer(index, taille, type, normalisation, écart, adresse)
    // (Déplacé dans Initialise avec le VAO)

    // etape e.

    // etape f. dessin de triangles dont la definition provient d’un tableau
    // le rendu s’effectue ici en prenant 3 sommets a partir du debut du tableau (0)

    int loc_transform = glGetUniformLocation(basicProgram, "u_transform");
    int loc_normalMatrix = glGetUniformLocation(basicProgram, "u_normalMatrix");
    int loc_modelMatrix = glGetUniformLocation(basicProgram, "u_modelMatrix");

    float time = glfwGetTime();
    float aspect = (float)width / (float)(height);
    Matrix4 proj = Perspective(3.14159265f / 3.0f, aspect, 0.1f, 100.0f);

    // Rendu du Cube
    {
        Matrix4 rotY = RotateY(time * 1.2f);
        Matrix4 rotX = RotateX(time * 0.8f);
        Matrix4 rot = Multiply(rotX, rotY);
        Matrix4 trans = Translate(-2.5f, 0.0f, -7.0f);
        Matrix4 transform = Multiply(trans, rot);
        Matrix4 mvp = Multiply(proj, transform);

        glUniformMatrix4fv(loc_transform, 1, GL_FALSE, mvp.m);
        glUniformMatrix4fv(loc_normalMatrix, 1, GL_FALSE, rot.m);
        glUniformMatrix4fv(loc_modelMatrix, 1, GL_FALSE, transform.m);

        // Liaison de la texture blanche par défaut
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
        Matrix4 rot = RotateY(time);
        Matrix4 trans = Translate(2.0f, -1.0f, -12.0f);
        Matrix4 scale = Scale(0.5f, 0.5f, 0.5f);
        Matrix4 transform = Multiply(trans, Multiply(rot, scale));
        Matrix4 mvp = Multiply(proj, transform);

        glUniformMatrix4fv(loc_transform, 1, GL_FALSE, mvp.m);
        glUniformMatrix4fv(loc_normalMatrix, 1, GL_FALSE, rot.m);
        glUniformMatrix4fv(loc_modelMatrix, 1, GL_FALSE, transform.m);

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