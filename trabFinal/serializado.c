//----------------------------------------------
//  Trabalho Final - Efeito Particula 
//  Gustavo Rodrigues Almeida       RA: 489999
//  Henrique Teruo Eihara           RA: 490016
//  Marcello da Costa Marques Acar  RA: 552550
//----------------------------------------------
//
// Programa Sequencial
//
// Para compilar:
// gcc serializado.c -o serial -lGL -lGLU -lglut -lm
//
// Para instalar as bibliotecas necessárias do glut utiliza-se:
//    sudo apt-get install freeglut3-dev
//
// Para executar o programa sem vsync, deve-se instalar:
//    sudo apt-get install mesa-utils
// e executar:
//    vblank_mode=0 ./serial <Número de Partículas>

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#include "timer.h"

// Número de partículas (informado pelo usuário)
int nParticulas;

// Número auxiliar de partículas (para saber se a execução do programa chegou ao fim, ou seja, se as partículas estão no chão)
int nParticulasAux;

// Variáveis necessárias para a realização
// Do cálculo de Frames Per Second
int tempo;
int frame = 0, timebase = 0;

// Flag utilizadas para a finalização do programa
int Flag = 0;

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
    vetor2[i*3] = handDouble(-7,7) * (rand() % 2 == 0 ? 1 :-1);
    vetor2[i*3+2] = handDouble(-7,7) * (rand() % 2 == 0 ? 1 :-1);
  }
}

// Função que calcula a nova posição das partículas
void * renderizarParticulas(){
  int i;

  nParticulasAux = 0;

  for(i=0;i<nParticulas;i++){
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
    // Caso a partícula esteja no chão, adiciona no auxiliar quantas partículas estão no chão
    if(vetor2[i*3+1] < -9)
      nParticulasAux++;
  }

  // Caso todas as partículas estejam no chão, 
  if(nParticulasAux == nParticulas)
    Flag = 1;

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

  // Chama a função para renderizar as partículas
  renderizarParticulas();

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

  if(argc != 2){
    printf("Usage: %s <number of particles>\n",argv[0]);
    return 0;
  }

  // Recebe o número de partículas que devem ser processadas
  // e posteriormente renderizadas
  nParticulas = atoi(argv[1]);

  // Define a quantidade de partículas que não estão no chão (inicialmente nenhuma delas está)
  nParticulasAux = nParticulas;

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

  // Registrando CallBacks
  glutDisplayFunc(renderScene);
  glutReshapeFunc(changeSize);
  glutIdleFunc(renderScene);

  // Função que mantém o loop do evento de renderização
  // funcionado
  // Foi escolhido a função glutMainLoopEvent ao invés de glutMainLoop
  // pelo simples fato de que a segunda não pára até o programa finalizar
  // e a que escolhemos funciona como apenas uma iteração dessa função
  while(!Flag)
  {
    glutMainLoopEvent();
    renderScene();
  }

  // Finaliza o timer
  GET_TIME(finish);

  // Printa o tempo
  printf("Tempo gasto: %f\n", (finish - start));

  free(vetor2);

  return 1;
}
