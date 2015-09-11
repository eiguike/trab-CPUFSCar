
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

#define N_ITENS 100
//#define TAM_SENHA 4
#define TAM_HASH 256
#define NUM_PRODUTOR 1
#define NUM_CONSUMIDOR 4

char * senhaAlvo;
char ** buffer;
char * senha;
char * auxiliar;

int TAM_SENHA;

int inicio = 0, final = 0, cont = 0, encontrada = 0, finalizada = 0;

pthread_mutex_t bloqueio;
pthread_cond_t nao_vazio, nao_cheio, encontrado;

int incrementa_senha() {
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
    return 0;
  }
  return 1;
}

void * produtor(void *v) {
  char ** variavelLocal = NULL;
  int i;
  int finalizado;
  do{
    if(encontrada)
      return NULL;

    // bloqueia o buffer para que o produtor
    // possa gerar mais senhas
    pthread_mutex_lock(&bloqueio);
    variavelLocal = (char**) malloc(sizeof(char*)*N_ITENS);

    for(i=0;i<N_ITENS;i++)
      variavelLocal[i] = NULL;

    i = 0;
    do{
      // local aonde se gera as chaves
      auxiliar = (char*)malloc(sizeof(char)*TAM_SENHA);
      strcpy(auxiliar, senha);
      variavelLocal[i] = auxiliar;
      i++;
    }while((i < N_ITENS)&&(finalizado = incrementa_senha()));

    pthread_mutex_unlock(&bloqueio);
    pthread_cond_signal(&nao_vazio);

    if(!finalizado)
      return NULL;

  }while(1);
}

void* consumidor(void *v) {
  // variavel local para que a thread
  // possa trabalhar com ela independentemnete
  // do buffer
  char ** variavelLocal2 = NULL;
  int i;

  do{
    // quando já foi encontrada a chave, é finalizado
    // thread
    if(encontrada)
      return NULL;

    // bloqueia o acesso de outras threads
    // no buffer
    pthread_mutex_lock(&bloqueio);
    variavelLocal2 = buffer;
    buffer = NULL;
    cont = 0;
    pthread_mutex_unlock(&bloqueio);

    for(i=0; i < N_ITENS; i++)
      if(variavelLocal2[i] != NULL)
        if(!testa_senha(senhaAlvo, variavelLocal2[i])){
          encontrada = 1;
          return NULL;
        }else{
          free(variavelLocal2[i]);
        }
    free(variavelLocal2);

    // desbloqueia para que produtor crie mais
    pthread_cond_signal(&nao_cheio);
    pthread_mutex_unlock(&bloqueio);
  }while(1);
}

int main(int argc, char *argv[]) {
  int i;

  // variáveis para controle do tempo de duração da execução do programa
  clock_t start = 0;
  clock_t finish = 0;

  // verifica o nÃºmero de argumentos
  if(argc != 3){
    printf("Usage: %s <número de digitos> <hash>\n",argv[0]);
    exit(0);
  }

  buffer = (char**) malloc(sizeof(char*)*N_ITENS);

  // recebe a senha
  TAM_SENHA = atoi(argv[1]);
  senha = (char*) malloc(sizeof(char)*(atoi(argv[1])));

  // definindo senha inicial
  for(i=0;i < TAM_SENHA;i++)
    senha[i] = '0';
  senha[TAM_SENHA] = '\0';

  senhaAlvo = (char*)malloc(sizeof(char)*(strlen(argv[2])));
  strcpy(senhaAlvo,argv[2]);

  // threads de produtor e consumidor respectivamente
  pthread_t thr_produtor[NUM_PRODUTOR], thr_consumidor[NUM_CONSUMIDOR];

  // cria os estados e o mutex
  pthread_mutex_init(&bloqueio, NULL);
  pthread_cond_init(&nao_cheio, NULL);
  pthread_cond_init(&nao_vazio, NULL);
  pthread_cond_init(&encontrado, NULL);

  // cria as threads
  start = clock();
  for(i = 0; i < NUM_PRODUTOR; i++)
    pthread_create(&thr_produtor[i], NULL, produtor, NULL);
  for(i = 0; i < NUM_CONSUMIDOR; i++)
    pthread_create(&thr_consumidor[i], NULL, consumidor, NULL);

  // finaliza as threads
  for(i=0; i < NUM_PRODUTOR; i++)
    pthread_join(thr_produtor[i], NULL);
  for(i=0; i < NUM_CONSUMIDOR; i++)
    pthread_join(thr_consumidor[i], NULL);

  finish = clock();
  if(encontrada){
    printf("%f\n",(float)(finish - start)/CLOCKS_PER_SEC);
  }

  // limpa o buffer e a senha
  free(senhaAlvo);
  free(*buffer);

  return 0;
}
