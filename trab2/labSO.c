
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
#define NUM_PRODUTOR 3
#define NUM_CONSUMIDOR 3

static int NUM_THREADS = NUM_PRODUTOR + NUM_CONSUMIDOR;

// variáveis para controle do tempo de duração da execução do programa
clock_t start = 0;
clock_t finish = 0;

int TAM_SENHA;

int encontrada;

int incrementa_senha(char * senha) {
  int i;

  i = TAM_SENHA - 1;
  while (i >= 0) {
    if (senha[i] != '9') {
      senha[i]++;
      i = -2;
    } else {
      senha[i] = '0';
      i--;
    }
  }

  if (i == -1) {
    return 0; // quando nÃ£o hÃ¡ mais como incrementar
  }else{
    return 1;
  }
}
void calcula_hash_senha(const char *senha, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(senha, "aa", &data));
}

int testa_senha(const char *hash_alvo, const char *senha) {
  char hash_calculado[TAM_HASH + 1];

  calcula_hash_senha(senha, hash_calculado);
  if (!strcmp(hash_alvo, hash_calculado)) {
    printf("ACHEI A SENHA!!!! %s\n", senha);
    return 0;
  }

  return 1;
}

void produtor(int rank) {

  printf("Produtor morreu...\n");
}

void consumidor() {

  printf("Consumidor morreu...\n");
}

int main(int argc, char *argv[]) {
  // verifica o nÃºmero de argumentos
  if(argc != 3){
    printf("Usage: %s <número de digitos> <hash>\n",argv[0]);
    exit(0);
  }

  // recebe a senha
  TAM_SENHA = atoi(argv[1]);

# pragma omp parallel num_threads(NUM_THREADS)
  if(omp_get_thread_num() < NUM_PRODUTOR){
    produtor(omp_get_thread_num());
  }else{
    consumidor();
  }

  finish = clock();
  if(encontrada){
    printf("%f\n",(float)(finish - start)/CLOCKS_PER_SEC);
  }

  // limpa o buffer e a senha
  return 0;
}

