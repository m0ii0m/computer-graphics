#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/GLShader.h"

struct Vector2D {
    float x, y;
};

struct Vector3D {
    float x, y, z;
};

struct Vertex {
    Vector2D position;
    Vector3D color;
};

static const Vertex triangle[] = {
    { {-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f} },
    { { 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f} },
    { { 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f} }
};

GLFWwindow* window;
GLShader g_BasicShader;
GLuint VBO;

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

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData alloue et transfere
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    // je recommande de reinitialiser les etats a la fin pour eviter les effets de bord
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void Terminate() {
    glDeleteBuffers(1, &VBO);

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

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    int loc_color = glGetAttribLocation(basicProgram, "a_color");
    glEnableVertexAttribArray(loc_color);
    glVertexAttribPointer(loc_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    // etape e.

    // etape f. dessin de triangles dont la definition provient d’un tableau
    // le rendu s’effectue ici en prenant 3 sommets a partir du debut du tableau (0)
    glDrawArrays(GL_TRIANGLES, 0, 3);
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