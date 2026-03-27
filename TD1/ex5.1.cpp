#include <GL/glut.h>
#include <GL/GLU.h>

void Render()
{
  glClearColor(1.f, 1.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  gluPerspective(45.f, 960.f / 540.f, 0.1f, 1000.f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(0.f, 5.f, 5.f,
            0.f, 0.f, 0.f,
            0.f, 1.f, 0.f);

  glTranslatef(0.2f, 0.f, -3.f);
  glRotatef(60.f, 0.f, 1.f, 0.f);
  glScalef(1.5f, 1.5f, 1.5f);

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
