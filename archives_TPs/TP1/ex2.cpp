#include <GL/glut.h>
void Render()
{
  glEnable(GL_SCISSOR_TEST);

  glViewport(0, 0, 480, 270);
  glScissor(0, 0, 480, 270);
  glClearColor(0.f, 1.f, 1.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_LINE_LOOP);
  glColor3f(0.f, 0.f, 0.f);
  glVertex2f(-0.5f, -0.5f);
  glVertex2f(0.5f, -0.5f);
  glVertex2f(-0.5f, 0.5f);
  glVertex2f(0.5f, 0.5f);
  glEnd();

  glViewport(480, 0, 480, 270);
  glScissor(480, 0, 480, 270);
  glClearColor(1.f, 0.f, 1.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_TRIANGLE_STRIP);
  glColor3f(1.f, 0.f, 0.f);
  glVertex2f(-0.5f, -0.5f);
  glVertex2f(0.5f, -0.5f);
  glColor3f(0.f, 1.f, 0.f);
  glVertex2f(-0.5f, 0.5f);
  glVertex2f(0.5f, 0.5f);
  glEnd();

  glViewport(0, 270, 480, 270);
  glScissor(0, 270, 480, 270);
  glClearColor(1.f, 1.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_TRIANGLES);
  glColor3f(0.f, 0.f, 1.f);
  glVertex2f(0.0f, 0.5f);
  glVertex2f(-0.5f, -0.5f);
  glVertex2f(0.5f, -0.5f);
  glEnd();

  glViewport(480, 270, 480, 270);
  glScissor(480, 270, 480, 270);
  glClearColor(0.5f, 0.5f, 0.5f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_TRIANGLE_FAN);
  glColor3f(0.f, 0.f, 0.f);
  glVertex2f(-0.5f, -0.5f);
  glVertex2f(0.5f, -0.5f);
  glColor3f(0.f, 1.f, 1.f);
  glVertex2f(-0.5f, 0.f);
  glVertex2f(0.5f, 0.f);
  glColor3f(1.f, 1.f, 0.f);
  glVertex2f(-0.5f, 0.5f);
  glVertex2f(0.5f, 0.5f);
  glEnd();
}

// la ‘callback’ executee par glutDisplayFunc()
void Display()
{
  Render();
  glutSwapBuffers();
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
