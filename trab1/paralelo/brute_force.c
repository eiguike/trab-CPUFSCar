#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <crypt.h>
#include <stdlib.h>
#include <time.h>

#include "fila.h"

// declarando biblioteca pthread
#include <pthread.h>

#define CONS_THREADS 3
#define PROD_THREADS 1
#define TAM_HASH 256
#define TAM_VECTOR 3

void CalculaHashSenha(const char *senha, char *hash);
int IncrementaSenha(char *senha);
int TestaSenha(const char *hash_alvo, const char *senha);

void start();
void * consumePassword();
void * createPassword();
void foundPassword();
void initializePassword();

// Mutex para bloquear quando a senha esta sendo gerada
pthread_mutex_t isTherePassword[TAM_VECTOR];

// Mutex para bloequar acesso quando a senha está sendo utilizada
// e não esta pronto para ser utilizada
pthread_mutex_t canIUseThePassword[TAM_VECTOR];

int tamanhoSenha = 0;

char ** senha;
char * actualPassword;

int passwordLength;

int main(int argc, char *argv[])
{
  if(argc < 3){
    printf("Uso: %s <tamanho da senha> <hash>", argv[0]);
    return 1;
  }

  // Define tamanho da senha passada como parâmetro
  passwordLength = atoi(argv[1]);

  int i;

  clock_t start = 0;
  clock_t finish = 0;

  long thread;
  pthread_t * thread_handles = malloc((CONS_THREADS+PROD_THREADS)*sizeof(pthread_t));
  start = clock();

  // Criação de threads de consumidores e produtores respectivamente
  for(i=0;i<CONS_THREADS;i++)
    pthread_create(&thread_handles[i], NULL, consumePassword, (void *) thread);
  for(i=0;i<PROD_THREADS;i++)
    pthread_create(&thread_handles[i+CONS_THREADS], NULL, createPassword, (void *) thread);

  // inicializando primeira senha a ser quebrada
  actualPassword = malloc((passwordLength + 1)*sizeof(char));
  memset(senha, '0', passwordLength);
  actualPassword[passwordLength] = '\0';

  // Limpeza das threads
  free(thread_handles);
  finish = clock();
}

void * consumePassword(){
  int i;

  for(i=0; i < TAM_VECTOR; i++){

  }

}

int TestaSenha(const char *hashAlvo, const char *senha) {
  int result = 0;
  char hashCalculado[TAM_HASH + 1];

  memset(hashCalculado, '0', TAM_HASH);
  hashCalculado[TAM_HASH] = '\0';

  CalculaHashSenha(senha, hashCalculado);

  if (!strcmp(hashAlvo, hashCalculado)) {
    result = 1;
  }

  return result;
}

void * createPassword(){
  int index = tamanhoSenha - 1;

  char * newPassword = (char*)malloc(sizeof(char)*passwordLength);


  while (index >= 0){
    if (senha[index] != '9'){
      senha[index]++;
      index = -2;
    }else
      senha[index] = '0';
    index--;
  }
}

void CalculaHashSenha(const char *senha, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(senha, "aa", &data));
}
