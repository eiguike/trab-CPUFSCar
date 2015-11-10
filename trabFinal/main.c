//
// para compilar:
// gcc main.c -o main -lGL -lGLU -lglut
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>

void changeSize(int w, int h) {

  // Prevent a divide by zero, when window is too short
  // (you cant make a window of zero width).
  if (h == 0)
    h = 1;

  float ratio =  w * 1.0 / h;

  // Use the Projection Matrix
  glMatrixMode(GL_PROJECTION);

  // Reset Matrix
  glLoadIdentity();

  // Set the viewport to be the entire window
  glViewport(0, 0, w, h);

  // Set the correct perspective.
  gluPerspective(45.0f, ratio, 0.1f, 100.0f);

  // Get Back to the Modelview
  glMatrixMode(GL_MODELVIEW);
}

float angle = 0.0f;

void renderScene(void) {

  // Clear Color and Depth Buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Reset transformations
  glLoadIdentity();
  // Set the camera
  gluLookAt(	100*sin(angle), 1.0f, cos(angle)+7.0f,
      0.0f, 0.0f,  0.0f,
      0.0f, 1.0f,  0.0f);
  glColor3d(1,1,1);

  glPushMatrix();
    glTranslated(0,-1.2,-6);
    glRotated(90,45,0,0);
    // tenho que fazer a rotação na câmera e não na esfera
    //glRotated(angle,0,0,1);
    glutWireSphere(3,16,16);
  glPopMatrix();

  angle+=0.1f;

  glutSwapBuffers();
}

int main(int argc, char **argv) {

  // init GLUT and create window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(320,320);
  glutCreateWindow("Lighthouse3D- GLUT Tutorial");

  // register callbacks
  glutDisplayFunc(renderScene);
  glutReshapeFunc(changeSize);
  glutIdleFunc(renderScene);

  // enter GLUT event processing cycle
  glutMainLoop();

  return 1;
}
