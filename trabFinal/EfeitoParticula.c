//----------------------------------------------
//  Trabalho Final - Efeito Particula
//  Gustavo Rodrigues Almeida       RA: 489999
//  Henrique Teruo Eihara           RA: 490016
//  Marcello da Costa Marques Acar  RA: 552550
//----------------------------------------------
//
// Programa Sequencial
// para compilar:
// gcc EfeitoParticula.c -o EfeitoParticula -lGL -lGLU -lglut -lm -lpthread
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
#include <pthread.h>

#include "timer.h"

typedef struct arg{
  int inicio;
  int final;
}Arg;

// número de particulas
int nParticulas;

// número de threads
int NTHREADS;
int threadsFinalizadas = 0;

// variáveis necessárias para realizar
// o cálculo de fps
int tempo;
int frame = 0, timebase = 0;

// variável que contem a posição em
// x, y e z da partícula
double * vetor2;

// função que retorna o valor aleatório de um intervaldo
// em double
double handDouble(double min, double max){
  double aux = (double)rand()/(double)RAND_MAX * (max - min) + min;
  return aux;
}

// função que inicia as posições de cada partícula
void iniciarParticula(){
  vetor2 = malloc(sizeof(double)*3*nParticulas);
  int i;

  for(i=0;i<nParticulas;i++){
    vetor2[i*3+1] = 10 + handDouble(0,30);
    vetor2[i*3] = handDouble(-6,6) * (rand() % 2 == 0 ? 1 :-1);
    vetor2[i*3+2] = handDouble(-6,6) * (rand() % 2 == 0 ? 1 :-1);
  }
}

// função que calcula a nova posição da partícula
void * renderizarParticulas(Arg * argumento){
  int i;
  int flag = 1;
  while(1 && flag){
    flag = 0;
    for(i=argumento->inicio;i<argumento->final;i++){
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
      if(vetor2[i*3+1] > -9)
        flag = 1;

    }
  }

  threadsFinalizadas++;

  return NULL;
}

// função que não permite que a imagem renderizada
// perca a proporção quando é redimensionado manualmente
// a janela
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

// função principal para realizar a renderização
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
  gluLookAt(10,-7.0f, 20,
      0.0, 0.0,  0.0,
      0.0f, 1.0f,  0.0f);

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

// rotina principal
int main(int argc, char **argv) {

  double start, finish;

  if(argc != 3){
    printf("Usage: %s <number of particles> <number of threads>\n",argv[0]);
    return 0;
  }

  // argumentos encapsulados para a passagem de parâmetros
  Arg * argumento = malloc(sizeof(Arg)*atoi(argv[2]));

  // recebe o número de threads que devem ser criadas
  NTHREADS = atoi(argv[2]);
  pthread_t * threads = malloc(sizeof(pthread_t)*NTHREADS);

  // recebe o número de partículas que devem ser processadas
  // e posteriormente renderizadas
  nParticulas = atoi(argv[1]);
  int divisao = nParticulas/NTHREADS;
  int i;

  argumento[0].inicio = 0;
  argumento[0].final = divisao;

  for(i=1;i<NTHREADS;i++){
    argumento[i].inicio = argumento[i-1].final +1;
    argumento[i].final = divisao*(i+1);
  }
  //argumento[i-1].final = nParticulas;

  // define posição inicial das particulas
  iniciarParticula();

  // funções necessárias para inicializar o opengl
  // e criar a janela que ficará renderizado
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(640,640);
  glutCreateWindow("Trabalho Final - Computação Paralela");
  glEnable(GL_DEPTH_TEST);

  GET_TIME(start);
  // chamada das threads
  for(i=0;i<NTHREADS;i++){
    pthread_create(&threads[i], NULL, (void*)renderizarParticulas, (Arg*)&argumento[i]);
  }

  // definindo as funções que devem ser executadas
  // para uma determinada ação
  glutReshapeFunc(changeSize);

  // função que mantém o loop de renderização
  while(NTHREADS > threadsFinalizadas){
    glutMainLoopEvent();
    renderScene();
  }

  for(i=0;i<NTHREADS; i++){
    pthread_join(threads[i], NULL);
  }
  GET_TIME(finish);

  printf("Tempo gasto: %f\n", (finish - start));

  return 1;
}
