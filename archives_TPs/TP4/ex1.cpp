#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/GLShader.h"
#include <cmath>

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

const GLfloat cube_vertices[] = {
    // front
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    // back
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f
};

const GLushort cube_elements[] = {
    // front
    0, 1, 2,  2, 3, 0,
    // right
    1, 5, 6,  6, 2, 1,
    // back
    7, 6, 5,  5, 4, 7,
    // left
    4, 0, 3,  3, 7, 4,
    // bottom
    4, 5, 1,  1, 0, 4,
    // top
    3, 2, 6,  6, 7, 3
};

GLFWwindow* window;
GLShader g_BasicShader;
GLuint VAO;
GLuint VBO;
GLuint IBO;

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
    g_BasicShader.LoadVertexShader("Basic.vs");
    g_BasicShader.LoadFragmentShader("Basic.fs");
    g_BasicShader.Create();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

    auto basicProgram = g_BasicShader.GetProgram();

    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // On unbind le VAO d'abord, pour sauvegarder l'état (y compris le binding de l'IBO et les attributs)
    glBindVertexArray(0);
    // je recommande de reinitialiser les etats a la fin pour eviter les effets de bord
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

void Terminate() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);

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
    glClear(GL_COLOR_BUFFER_BIT);

    // etape c. on specifie le shader program a utiliser
    auto basicProgram = g_BasicShader.GetProgram();
    glUseProgram(basicProgram);

    // etape d.
    // glVertexAttribPointer(index, taille, type, normalisation, écart, adresse)
    // (Déplacé dans Initialise avec le VAO)

    // etape e.

    // etape f. dessin de triangles dont la definition provient d’un tableau
    // le rendu s’effectue ici en prenant 3 sommets a partir du debut du tableau (0)
    
    float time = glfwGetTime();
    
    // Les rotations doivent suivre l’ordre suivant : autour de l’axe Up, puis autour de l’axe Right, puis autour de l’axe Forward.
    Matrix4 rotY = RotateY(time * 1.2f);
    Matrix4 rotX = RotateX(time * 0.8f);
    Matrix4 rotZ = RotateZ(time * 0.5f);
    
    Matrix4 rot = Multiply(rotX, rotY);
    rot = Multiply(rotZ, rot);
    
    // On recule le cube
    Matrix4 trans = Translate(0.0f, 0.0f, -5.0f);
    Matrix4 scale = Scale(1.0f, 1.0f, 1.0f);

    // transform = trans * rot * scale
    Matrix4 transform = Multiply(trans, rot);
    transform = Multiply(transform, scale);

    // Projection Perspective
    float aspect = (float)width / (float)(height);
    Matrix4 proj = Perspective(3.14159265f / 3.0f, aspect, 0.1f, 100.0f);

    // proj * transform
    transform = Multiply(proj, transform);

    int loc_transform = glGetUniformLocation(basicProgram, "u_transform");
    glUniformMatrix4fv(loc_transform, 1, GL_FALSE, transform.m);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
    // on suppose que la phase d’echange des buffers front et back
    // le « swap buffers » est effectuee juste apres
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