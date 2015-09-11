
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

// Biblioteca que possui a função clock();
#include <time.h>

#define N_ITENS 3000
//#define TAM_SENHA 4
#define TAM_HASH 256
#define NUM_PRODUTOR 1
#define NUM_CONSUMIDOR 4

int consumidorNumeros = 0;
int encontrada;
int divisao;
int TAM_SENHA;
char * senha;
char hashEntrada[TAM_HASH];
clock_t start;
clock_t finish;

int incrementa_senha(char * pass) {
  int i;

  i = TAM_SENHA - 1;
  while (i >= 0) {
    if (pass[i] != '9') {
      pass[i]++;
      i = -2;
    } else {
      pass[i] = '0';
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
    return 0;
  }
  //printf("%s %s %s\n",hash_alvo, hash_calculado, senha);
  return 1;
}

void* consumidor(void *n) {
  int ene = *(int*) n;
  int i;

  char * senha = (char*) malloc(sizeof(char)*TAM_SENHA);
  char * aux = (char*) malloc(sizeof(char)*TAM_SENHA);
  if(ene == 0){
    for(i=0;i<TAM_SENHA;i++)
      senha[i] = '0';
  }else{
    sprintf(aux, "%d", ene);
    int conta = strlen(aux) - TAM_SENHA;
    for(i=0;i<conta;i++)
      senha[i] = '0';
    strcat(senha,aux);
  }
  senha[TAM_SENHA] ='\0';
  for(i=0;(i<divisao+1) && !encontrada; i++){
    if(testa_senha(hashEntrada, senha) == 0){
      // deu certo
      encontrada = 1;
      return NULL;
    }else{
      if(!incrementa_senha(senha)){
        encontrada = 0;
        return NULL;
      }
    }
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  int i;

  // verifica o nÃºmero de argumentos
  if(argc != 3){
    printf("Usage: %s <número de digitos> <hash>\n",argv[0]);
    exit(0);
  }

  // recebe a senha
  TAM_SENHA = atoi(argv[1]);
  senha = (char*) malloc(sizeof(char)*(atoi(argv[1])));
  strcpy(hashEntrada, argv[2]);

  for(i=0;i < TAM_SENHA;i++)
    senha[i] = '9';
  senha[TAM_SENHA] = '\0';

  divisao = atoi(senha);
  divisao = divisao/4;

  int comecoThread[4];
  comecoThread[0] = 0;
  comecoThread[1] = divisao +1;
  comecoThread[2] = comecoThread[1] + divisao +1;
  comecoThread[3] = comecoThread[2] + divisao +1;

  pthread_t thr_consumidor[NUM_CONSUMIDOR];

  start = clock();
  for(i = 0; i < NUM_CONSUMIDOR; i++)
    pthread_create(&thr_consumidor[i], NULL, consumidor, &comecoThread[i]);

  for(i=0; i < NUM_CONSUMIDOR; i++)
    pthread_join(thr_consumidor[i], NULL);
  finish = clock();

  if(encontrada){
    printf("%f\n",(float)(finish - start)/CLOCKS_PER_SEC);
  }

  return 0;
}
