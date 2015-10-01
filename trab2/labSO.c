
/*----------------------------------
 * LaboratÃ³rio de Sistemas Operacionais
 * Projeto 4: ProgramaÃ§Ã£o Concorrente
 * Nome: Henrique Teruo Eihara          RA: 490016
 * Nome: Marcello da Costa Marques Acar RA: 552550
 ----------------------------------- */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <crypt.h>
#include <omp.h>

// fila
#include "estrutura.h"

// Biblioteca que possui a função clock();
#include <time.h>

#define N_ITENS 2000
//#define TAM_SENHA 4
#define TAM_HASH 256

int NUM_THREADS = 1;

// variáveis para controle do tempo de duração da execução do programa
clock_t start = 0;
clock_t finish = 0;

int TAM_SENHA;

int encontrada = 0;

int * vetorInicio;
int * vetorFinal;

void calcula_hash_senha(const char *senha, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(senha, "aa", &data));
}

int testa_senha(const char *hash_alvo, const char *senha) {
  char hash_calculado[TAM_HASH + 1];

  //printf("%d: TESTEI A SENHA %s\n", omp_get_thread_num(), senha);
  //printf("%s\n", senha);

  calcula_hash_senha(senha, hash_calculado);
  if (!strcmp(hash_alvo, hash_calculado)) {
    printf("ACHEI A SENHA!!!! %s\n", senha);
    return 0;
  }
  return 1;
}

void produtor(char * hash) {
  int meuId = omp_get_thread_num();
  char * senha = malloc(sizeof(char)*TAM_SENHA);
  char * senhaAux = malloc(sizeof(char)*TAM_SENHA);
  int overflow;

  int i;

  for(i = 0; i < (TAM_SENHA); i++){
      senha[i] = '0';
  }
  sprintf(senhaAux,"%d", vetorInicio[meuId]);
  strcpy(&senha[TAM_SENHA - strlen(senhaAux)], senhaAux);

  while(!encontrada && vetorInicio[meuId] <= vetorFinal[meuId]){
    for(i=TAM_SENHA-1;(overflow==1)&&(i>=0); i--)
      if(senha[i] >= 57){
        senha[i]+= -9;
        overflow = 1;
      }else{
        if(senha[i] >= 48){
          senha[i]++;
          overflow = 0;
        }
      }

      //senha[TAM_SENHA-i] += (overflow) && (senha[TAM_SENHA-i] < 57) ? overflow--: (overflow=1),-9;
    senha[TAM_SENHA] = '\0';
    //printf("%s\n",senha);

    if(!testa_senha(hash, senha)){
      encontrada = 1;
      break;
    }
    vetorInicio[meuId]++;
    overflow = 1;
  }

  free(senha);
  free(senhaAux);
  printf("Thread morreu...\n");
}

int main(int argc, char *argv[]) {
  int i;
  // verifica o nÃºmero de argumentos
  if(argc != 4){
    printf("Usage: %s <número de digitos> <hash> <número de threads>\n",argv[0]);
    exit(0);
  }

  NUM_THREADS = atoi(argv[3]);

  // recebe a senha
  TAM_SENHA = atoi(argv[1]);

  // vetor que conterá as primeiras senhas que
  // devem ser geradas
  vetorInicio = malloc(sizeof(int)*NUM_THREADS);
  vetorFinal = malloc(sizeof(int)*NUM_THREADS);

  char * ultimaSenha = malloc(sizeof(char)*TAM_SENHA);
  for(i=0;i<TAM_SENHA;i++)
    ultimaSenha[i] = '9';
  ultimaSenha[i] = '\0';

  int divisao = (atoi(ultimaSenha)+1)/NUM_THREADS;

  vetorInicio[0] = 0;
  vetorFinal[0] = divisao -1;
  for(i = 1; i < NUM_THREADS; i++){
    vetorInicio[i] = vetorInicio[i-1] + divisao;
    vetorFinal[i] = vetorInicio[i] + (divisao -1);
  }

  // tratamento de caso especial
  vetorFinal[NUM_THREADS - 1] += atoi(ultimaSenha) - vetorFinal[NUM_THREADS -1];

# pragma omp parallel num_threads(NUM_THREADS)
  produtor(argv[2]);

  finish = clock();
  if(encontrada){
    printf("%f\n",(float)(finish - start)/CLOCKS_PER_SEC);
  }

  free(ultimaSenha);
  free(vetorInicio);
  free(vetorFinal);

  // limpa o buffer e a senha
  return 0;
}

