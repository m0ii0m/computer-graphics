#include <GL/glut.h>
#include <cmath>
#include <array>

std::array<float, 16> getTranslateMatrix(float x, float y, float z)
{
    return {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        x, y, z, 1.f
    };
}

std::array<float, 16> getRotateMatrix(float angleDegrees, float x, float y, float z)
{
    float angleRadians = angleDegrees * (3.14159265358979323846f / 180.0f);
    float c = cosf(angleRadians);
    float s = sinf(angleRadians);
    float invC = 1.0f - c;

    return {
        c + x * x * invC,
        y * x * invC + z * s,
        z * x * invC - y * s,
        0.f,

        x * y * invC - z * s,
        c + y * y * invC,
        z * y * invC + x * s,
        0.f,

        x * z * invC + y * s,
        y * z * invC - x * s,
        c + z * z * invC,
        0.f,

        0.f, 0.f, 0.f, 1.f
    };
}

std::array<float, 16> getScaleMatrix(float x, float y, float z)
{
    return {
        x, 0.f, 0.f, 0.f,
        0.f, y, 0.f, 0.f,
        0.f, 0.f, z, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
}

void Render()
{
  glClearColor(1.f, 1.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();

  auto translate = getTranslateMatrix(0.2f, 0.f, 0.f);
  auto rotate = getRotateMatrix(90.f, 0.f, 0.f, 1.f);
  auto scale = getScaleMatrix(1.5f, 1.5f, 1.5f);
  glMultMatrixf(translate.data());
  glMultMatrixf(rotate.data());
  glMultMatrixf(scale.data());

  glBegin(GL_TRIANGLES);
  
  glColor3f(1.f, 0.f, 0.f);
  glVertex2f(0.0f, 0.5f);

  glColor3f(0.f, 0.f, 1.f);
  glVertex2f(-0.5f, -0.5f);

  glColor3f(0.f, 1.f, 0.f);
  glVertex2f(0.5f, -0.5f);

  glEnd();
}

// la ‘callback’ executee par glutDisplayFunc()
void Display()
{
  Render();
  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutInitWindowSize(960,540);
  glutCreateWindow("Triangle");
  glutDisplayFunc(Display);
  glutMainLoop();
  return 0;
}
