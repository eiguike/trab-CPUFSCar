//----------------------------------------------
//  Trabalho Final - Efeito Particula
//  Gustavo Rodrigues Almeida       RA: 489999
//  Henrique Teruo Eihara           RA: 490016
//  Marcello da Costa Marques Acar  RA: 552550
//----------------------------------------------
//
// Programa Paralelo
//
// Para compilar:
// gcc EfeitoParticula.c -o paralelo -lGL -lGLU -lglut -lm -lpthread
//
// Para instalar as bibliotecas necessárias do glut utiliza-se:
//    sudo apt-get install freeglut3-dev
//
// Para executar o programa sem vsync, deve-se instalar:
//    sudo apt-get install mesa-utils
// e executar:
//    vblank_mode=0 ./paralelo <Número de Partículas> <Número de Threads>

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#include "timer.h"

// Definição da Estrutura que é utilizada
typedef struct arg{
  int inicio;
  int final;
}Arg;

// Número de partículas (informado pelo usuário)
int nParticulas;

// Número de threads (informado pelo usuário)
int NTHREADS;

// Variável auxiliar das threads
// define o número de threads que finalizaram a execução
int threadsFinalizadas = 0;

// Variáveis necessárias para a realização
// Do cálculo de Frames Per Second
int tempo;
int frame = 0, timebase = 0;

// Vetor que contêm as variáveis
// x, y e z da partícula (em linha)
// Ou seja, ele guarda {x1,y1,z1,x2,y2,z2,...,xn,yn,zn}
double * vetor2;

// Função que retorna o valor aleatório
// dentro de um intervalo definido
double handDouble(double min, double max){
  double aux = (double)rand()/(double)RAND_MAX * (max - min) + min;
  return aux;
}

// Função que inicia as coordenadas da partícula aleatóriamente (dentro de um intervalo definido)
// sendo esse intervalo um plano acima da esfera
void iniciarParticula(){
  vetor2 = malloc(sizeof(double)*3*nParticulas);
  int i;

  for(i=0;i<nParticulas;i++){
    vetor2[i*3+1] = 10 + handDouble(0,30);
    vetor2[i*3] = handDouble(-6,6) * (rand() % 2 == 0 ? 1 :-1);
    vetor2[i*3+2] = handDouble(-6,6) * (rand() % 2 == 0 ? 1 :-1);
  }
}

// Função que calcula a nova posição das partículas
void * renderizarParticulas(Arg * argumento){
  int i;
  int flag = 1;
  int meuID = 0;

  while(1 && flag){
    flag = 0;

    for(i=argumento->inicio;i<argumento->final;i++){
      int raio = (vetor2[i*3]*vetor2[i*3])+(vetor2[i*3+1]*vetor2[i*3+1])+(vetor2[i*3+2]*vetor2[i*3+2]);

      // Verifica aonde a partícula se encontra com relação a esfera
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
      // Caso a partícula não esteja no chão, aciona a flag que define se o cálculo deve ser executado novamente
      if(vetor2[i*3+1] > -9)
        flag = 1;
    }
  }

  threadsFinalizadas++;

  return NULL;
}

// Função que não permite a perda de resolução
// quando o usuário redimensiona a tela manualmente
void changeSize(int w, int h) {
  // Previne a divisão por zero quando a janela é muito pequena
  // (não existe janelas com largura 0)
  if (h == 0)
    h = 1;

  float ratio =  w * 1.0 / h;

  // Usa a matriz de Projeção
  glMatrixMode(GL_PROJECTION);

  // Reseta a matriz
  glLoadIdentity();

  // Seta a viewport para ser a janela inteira
  glViewport(0, 0, w, h);

  // Define a perspectiva
  gluPerspective(45.0f, ratio, 0.1f, 100.0f);

  // Volta para o ModelView
  glMatrixMode(GL_MODELVIEW);
}

// Função principal de renderização da imagem
void renderScene(void) {
  char s [100];
  int i;
  void * font;

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_DOUBLE, 0, vetor2);

  // Limpa os Buffers de visão e cor
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3d(1,1,1);
  glDrawArrays(GL_POINTS, 0, nParticulas);
  // Reseta as transformações
  glLoadIdentity();
  // Define a posição da câmera
  gluLookAt(10,-7.0f, 20,
      0.0, 0.0,  0.0,
      0.0f, 1.0f,  0.0f);

  // Função que faz o cálculo dos Frames por Segundo
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

  // Troca os buffers (o atual com o antigo que fora recalculado)
  glutSwapBuffers();
}

// Rotina Principal do programa
int main(int argc, char **argv) {

  double start, finish;
  int i;

  if(argc != 3){
    printf("Usage: %s <number of particles> <number of threads>\n",argv[0]);
    return 0;
  }

  // Argumentos encapsulados para a passagem dos parâmetros
  Arg * argumento = malloc(sizeof(Arg)*atoi(argv[2]));

  // Recebe o número de threads que devem ser criadas
  NTHREADS = atoi(argv[2]);
  pthread_t * threads = malloc(sizeof(pthread_t)*NTHREADS);

  // Recebe o número de partículas que devem ser processadas
  // e posteriormente renderizadas
  nParticulas = atoi(argv[1]);

  // É feita a divisão de trabalho das threads
  int divisao = nParticulas/NTHREADS;

  argumento[0].inicio = 0;
  argumento[0].final = divisao;

  for(i=1;i<NTHREADS;i++){
    argumento[i].inicio = argumento[i-1].final +1;
    argumento[i].final = divisao*(i+1);
  }

  // Define posição inicial das particulas
  iniciarParticula();

  // Funções necessárias para inicializar o OpenGL
  // e criar a janela que ficará renderizando
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(640,640);
  glutCreateWindow("Trabalho Final - Computação Paralela");
  glEnable(GL_DEPTH_TEST);

  // Inicializa a contagem do tempo
  GET_TIME(start);

  // Chamada de criação de todas as threads
  // com a passagem dos argumentos (trabalho para cada uma delas)
  for(i=0;i<NTHREADS;i++){
    pthread_create(&threads[i], NULL, (void*)renderizarParticulas, (Arg*)&argumento[i]);
  }

  // Registrando CallBacks
  glutReshapeFunc(changeSize);

  // Função que mantêm a renderização em um loop
  // até que os cálculos das threads sejam finalizados (partículas se encontrem no chão)
  while(NTHREADS > threadsFinalizadas)
  {
    glutMainLoopEvent();
    renderScene();
  }

  // Aguarda o retorno de todas as threads que finalizaram sua execução
  for(i=0;i<NTHREADS; i++){
    pthread_join(threads[i], NULL);
  }

  // Finaliza o timer
  GET_TIME(finish);

  // Printa o tempo
  printf("Tempo gasto: %f\n", (finish - start));

  free(argumento);
  free(vetor2);
  free(threads);

  return 1;
}
