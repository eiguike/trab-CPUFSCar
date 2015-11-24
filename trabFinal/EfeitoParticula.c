//----------------------------------------------
//  Trabalho Final - Efeito Particula 
//  Gustavo Rodrigues Almeida       RA: 489999
//  Henrique Teruo Eihara           RA: 490016
//  Marcello da Costa Marques Acar  RA: 552550
//----------------------------------------------
//
// Programa Sequencial
//
// para compilar:
// gcc main.c -o main -lGL -lGLU -lglut -lm
//
// para instalar as bibliotecas necessárias
// sudo apt-get install freeglut3-dev
//
// instale este aplicativo:
// sudo apt-get install mesa-utils
// e execute sem  vsync:
// vblank_mode=0 ./main 1000

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

int nParticulas;

int tempo;
int frame = 0, timebase = 0;

double * vetor2;

typedef struct particula{
  double x;
  double y;
  double z;
  double velocity;
} Particula;

Particula * vetor;

double handDouble(double min, double max){
  double aux = (double)rand()/(double)RAND_MAX * (max - min) + min;
  return aux;
}

// função que inicia as posições de cada partícula
void iniciarParticula(){
  vetor = malloc(sizeof(Particula)*nParticulas);

  vetor2 = malloc(sizeof(double)*3*nParticulas);

  int i;

  for(i=0;i<nParticulas;i++){
    vetor2[i*3+1] = 10 + handDouble(0,30);
    vetor2[i*3] = handDouble(-7,7) * (rand() % 2 == 0 ? 1 :-1);
    vetor2[i*3+2] = handDouble(-7,7) * (rand() % 2 == 0 ? 1 :-1);
  }
}

void *  renderizarParticulas(){
  int i;
  while(1){
    for(i=0;i<nParticulas;i++){
      // colisão com a esfera
      int raio = (vetor2[i*3]*vetor2[i*3])+(vetor2[i*3+1]*vetor2[i*3+1])+(vetor2[i*3+2]*vetor2[i*3+2]);

      if(raio > 36){
        vetor2[i*3+1] =  vetor2[i*3+1] - (vetor2[i*3+1] < -9 ? 0 : handDouble(0.04,0.2));
      }else{
        vetor2[i*3+1] =  vetor2[i*3+1] - (vetor2[i*3+1] < -9 ? 0 : handDouble(0.04,0.04));
        if (vetor2[i*3] > 0){
          vetor2[i*3] = vetor2[i*3] + handDouble(0,0.1);
          if(vetor2[i*3+2] == 0)
            vetor2[i*3+2] = 0;
          else{
            if (vetor2[i*3+2] > 0)
              vetor2[i*3+2] = vetor2[i*3+2] + handDouble(0,0.1);
            else
              vetor2[i*3+2] = vetor2[i*3+2] - handDouble(0,0.1);
          }
        }
        else if (vetor2[i*3] < 0){
          vetor2[i*3] = vetor2[i*3] - handDouble(0,0.1);
          if(vetor2[i*3+2] == 0)
            vetor2[i*3+2] = 0;
          else{
            if (vetor2[i*3+2] > 0)
              vetor2[i*3+2] = vetor2[i*3+2] + handDouble(0,0.1);
            else
              vetor2[i*3+2] = vetor2[i*3+2] - handDouble(0,0.1);
          }
        }
        else{
          if(vetor2[i*3+2] == 0)
            vetor2[i*3] = 0;
          else{
            if (vetor2[i*3+2] > 0)
              vetor2[i*3+2] = vetor2[i*3+2] + handDouble(0,0.1);
            else
              vetor2[i*3+2] = vetor2[i*3+2] - handDouble(0,0.1);
          }
        }

      }
    }


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
  char s [100];
  int i;
  void * font;

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_DOUBLE, 0, vetor2);

  // Clear Color and Depth Buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3d(1,1,1);

  glDrawArrays(GL_POINTS, 0, nParticulas);
  // Reset transformations
  glLoadIdentity();
  // Set the camera
  gluLookAt(6,20.0f, 30,
      0.0, 0.0,  0.0,
      0.0f, 1.0f,  0.0f);
  glColor3d(1,1,1);

  glColor3d(0,1,0);
  glPushMatrix();
  //glRotated(90,45,0,0);
  // tenho que fazer a rotação na câmera e não na esfera
  //glRotated(angle,1,1,1);
  glutSolidSphere(6,100,100);
  glPopMatrix();

  angle+=0.1f;

  frame++;
  tempo = glutGet(GLUT_ELAPSED_TIME);
  if (tempo - timebase > 1000){
    sprintf(s,"FPS: %4.2f", frame * 1000.0/(tempo-timebase));
    printf("%s\n",s);
    timebase = tempo;
    frame = 0;
  }else{
    frame++;
  }

  glutSwapBuffers();
}

int main(int argc, char **argv) {
  pthread_t threads[2];

  if(argc != 3){
    printf("Usage: %s <number of particles> <number of threads>\n",argv[0]);
    return 0;
  }

  printf("THREADS: %s\n",argv[2]);

  nParticulas = atoi(argv[1]);
  srand(time(NULL));
  iniciarParticula();

  // init GLUT and create window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(640,640);
  glutCreateWindow("Trabalho Final - Computação Paralela");
  glEnable(GL_DEPTH_TEST);


  pthread_create(&threads[0], NULL, renderizarParticulas, NULL);

  // register callbacks
  glutDisplayFunc(renderScene);
  glutReshapeFunc(changeSize);
  glutIdleFunc(renderScene);

  // enter GLUT event processing cycle
  glutMainLoop();

  return 1;
}
