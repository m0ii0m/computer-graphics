#include <GL/glut.h>
#include <GL/GLU.h>

void Render()
{
  glClearColor(1.f, 1.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();

  gluOrtho2D(0.f, 960.f, 0.f, 540.f);

  glTranslatef(480.f, 270.f, 0.f);
  glRotatef(180.f, 0.f, 0.f, 1.f);
  glScalef(500.f, 500.f, 500.f);

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
