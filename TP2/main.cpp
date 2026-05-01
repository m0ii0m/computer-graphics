#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/GLShader.h"

GLShader g_BasicShader;

static const float triangle[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.0f,  0.5f
};

bool Initialise()
{
    g_BasicShader.LoadVertexShader("Basic.vs");
    g_BasicShader.LoadFragmentShader("Basic.fs");
    g_BasicShader.Create();

    return true;
}

void Terminate() {
    g_BasicShader.Destroy();
}

void Render(GLFWwindow* window)
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
    glEnableVertexAttribArray(0);
    // glVertexAttribPointer(index, taille, type, normalisation, écart, adresse)
    int loc_position = glGetAttribLocation(basicProgram, "a_position");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 2, GL_FLOAT, false, sizeof(float) * 2, triangle);

    // etape e.

    // etape f. dessin de triangles dont la definition provient d’un tableau
    // le rendu s’effectue ici en prenant 3 sommets a partir du debut du tableau (0)
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // on suppose que la phase d’echange des buffers front et back
    // le « swap buffers » est effectuee juste apres
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        glfwTerminate();
        return -1;
    }

    if (!Initialise())
    {
        glfwTerminate();
        return -1;
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        Render(window);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    Terminate();
    glfwTerminate();
    return 0;
}