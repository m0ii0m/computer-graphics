#include <GL/glut.h>
void Render()
{
  glClearColor(1.f, 1.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glPushMatrix();

  glRotatef(glutGet(GLUT_ELAPSED_TIME) / 1000.f * 90.f, 0.f, 0.f, 1.f);

  glBegin(GL_TRIANGLES);
  
  glColor3f(1.f, 0.f, 0.f);
  glVertex2f(0.0f, 0.5f);

  glColor3f(0.f, 0.f, 1.f);
  glVertex2f(-0.5f, -0.5f);

  glColor3f(0.f, 1.f, 0.f);
  glVertex2f(0.5f, -0.5f);

  glEnd();

  glPopMatrix();

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
