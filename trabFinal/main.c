//
// para compilar:
// gcc main.c -o main -lGL -lGLU -lglut -lm
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int nParticulas;

typedef struct particula{
  double x;
  double y;
  double z;
  double velocity;
} Particula;

Particula * vetor;

double funcaoBonita(double min, double max){
  double aux = (double)rand()/(double)RAND_MAX * (max - min) + min;
  return aux;
}

void iniciarParticula(){
  vetor = malloc(sizeof(Particula)*nParticulas);
  int i;

  for(i=0;i<nParticulas;i++){
    vetor[i].y = 10 + funcaoBonita(0,6);
    vetor[i].x = funcaoBonita(-6,6) * (rand() % 2 == 0 ? 1 :-1);
    vetor[i].z = funcaoBonita(-6,6) * (rand() % 2 == 0 ? 1 :-1);
  }
}

void renderizarParticulas(){
  int i;

  for(i=0;i<nParticulas;i++){
    glPushMatrix();
      glTranslated(vetor[i].x,vetor[i].y,vetor[i].z);
      // tenho que fazer a rotação na câmera e não na esfera
      //glRotated(angle,0,0,1);
      glutWireCube(0.1);
    glPopMatrix();
    vetor[i].y = vetor[i].y - (vetor[i].y < -6 ? 0 : funcaoBonita(-0.2,0.4));
  }

}

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

  float z = cos(angle) * 3;
  float x = sin(angle) * 3;

  // Clear Color and Depth Buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Reset transformations
  glLoadIdentity();
  // Set the camera
  gluLookAt(6,20.0f, 30,
      0.0, 0.0,  0.0,
      0.0f, 1.0f,  0.0f);
  glColor3d(1,1,1);

  renderizarParticulas();

  glColor3d(0,1,1);
  glPushMatrix();
    //glTranslated(1,1,1);
    glRotated(90,45,0,0);
    // tenho que fazer a rotação na câmera e não na esfera
    //glRotated(angle,0,0,1);
    glutSolidSphere(6,10,10);
  glPopMatrix();

  angle+=0.1f;

  glutSwapBuffers();
}

int main(int argc, char **argv) {
  nParticulas = atoi(argv[1]);
  srand(time(NULL));
  iniciarParticula();
  // init GLUT and create window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(320,320);
  glutCreateWindow("Lighthouse3D- GLUT Tutorial");
  glEnable(GL_DEPTH_TEST);

  // register callbacks
  glutDisplayFunc(renderScene);
  glutReshapeFunc(changeSize);
  glutIdleFunc(renderScene);

  // enter GLUT event processing cycle
  glutMainLoop();

  return 1;
}
