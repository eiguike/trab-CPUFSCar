
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

//#define TAM_SENHA 4
#define TAM_HASH 256
#define NUM_PRODUTOR 2
#define NUM_CONSUMIDOR 3

char * senhaAlvo;
char * buffer_um;
char * buffer_dois;
char * buffer_tres;
char * senha_crescente;
char * senha_decrescente;
char * auxiliar;

int TAM_SENHA;

int inicio = 0, final = 0, cont = 0, encontrada = 0, finalizada = 0;

pthread_mutex_t senhas, senha_cresc, senha_decresc, buffer1, buffer2, buffer3;
pthread_cond_t livre, encontrado;

int incrementa_senha() {
  int i;

  i = TAM_SENHA - 1;
  while (i >= 0) {
    if (senha_crescente[i] != '9') {
      senha_crescente[i]++;
      i = -2;
    } else {
      senha_crescente[i] = '0';
      i--;
    }
  }
  if (i == -1) {
    return 0; // quando nÃ£o hÃ¡ mais como incrementar
  }else{
    return 1;
  }
}

// int decrementa_senha(){
   
// }

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
  do{

    if(pthread_mutex_trylock(&senha_cresc) == 0)
    {
      pthread_cond_wait(&livre, &senha_cresc);
      printf("oi\n");

      auxiliar = (char*)malloc(sizeof(char)*TAM_SENHA);

      incrementa_senha();
      strcpy(auxiliar, senha_crescente);
      pthread_mutex_unlock(&senha_cresc);
    }
    else if(pthread_mutex_trylock(&senha_decresc) == 0)
    {
      pthread_cond_wait(&livre, &senha_decresc);
      printf("tchau\n");

      auxiliar = (char*)malloc(sizeof(char)*TAM_SENHA);

      // decrementa_senha();
      strcpy(auxiliar, senha_decrescente);
      pthread_mutex_unlock(&senha_decresc);
    }

    if(encontrada){
      pthread_cond_wait(&encontrado, &senhas);
      pthread_mutex_unlock(&senhas);
      return NULL;
    }

    //Verifica qual buffer está livre para receber
    if(pthread_mutex_trylock(&buffer1) == 0)
    {
      buffer_um = auxiliar;
      pthread_mutex_unlock(&buffer1);
    }
    else if(pthread_mutex_trylock(&buffer2) == 0)
    {
      buffer_dois = auxiliar;
      pthread_mutex_unlock(&buffer2);
    }
    else if(pthread_mutex_trylock(&buffer3) == 0)
    {
      buffer_tres = auxiliar;
      pthread_mutex_unlock(&buffer3);
    }

  }while(encontrada != 1);

  return NULL;
}

void* consumidor(void *v) {
  while(encontrada != 1)
  {
    //Testa se o Buffer1 está livre
    if(pthread_mutex_trylock(&buffer1) == 0)
    {
      //Testa se a senha do Buffer1 é a procurada
      if(testa_senha(senhaAlvo, buffer_um) == 1)
      {
        //Caso seja encontrada, marca a flag e envia um sinal de encontrado aos produtores.
        encontrada = 1;
        pthread_cond_signal(&encontrado);
        return NULL;
      }
      else
      {
        //Caso não seja encontrada, libera os buffers e marca como livre
        pthread_mutex_unlock(&buffer1);
        pthread_cond_signal(&livre);
      }
    }
    //Testa se o Buffer2 está livre
    else if(pthread_mutex_trylock(&buffer2) == 0)
    {
      //Testa se a senha do Buffer2 é a procurada
      if(testa_senha(senhaAlvo, buffer_dois) == 1)
      {
        //Caso seja encontrada, marca a flag e envia um sinal de encontrado aos produtores.
        encontrada = 1;
        pthread_cond_signal(&encontrado);
        return NULL;
      }
      else
      {
        //Caso não seja encontrada, libera os buffers e marca como livre
        pthread_mutex_unlock(&buffer2);
        pthread_cond_signal(&livre);
      }
    }
    //Testa se o Buffer3 está livre
    else if(pthread_mutex_trylock(&buffer3) == 0)
    {
      //Testa se a senha do Buffer3 é a procurada
      if(testa_senha(senhaAlvo, buffer_tres) == 1)
      {
        //Caso seja encontrada, marca a flag e envia um sinal de encontrado aos produtores.
        encontrada = 1;
        pthread_cond_signal(&encontrado);
        return NULL;
      }
      else
      {
        //Caso não seja encontrada, libera os buffers e marca como livre
        pthread_mutex_unlock(&buffer3);
        pthread_cond_signal(&livre);
      }
    }
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  char* senha;
  int i;

  // variáveis para controle do tempo de duração da execução do programa
  clock_t start = 0;
  clock_t finish = 0;

  // verifica o nÃºmero de argumentos
  if(argc != 3){
    printf("Usage: %s <número de digitos> <hash>\n",argv[0]);
    exit(0);
  }

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
  pthread_mutex_init(&senhas, NULL);
  pthread_mutex_init(&senha_cresc, NULL);
  pthread_mutex_init(&senha_decresc, NULL);
  pthread_mutex_init(&buffer1, NULL);
  pthread_mutex_init(&buffer2, NULL);
  pthread_mutex_init(&buffer3, NULL);
  pthread_cond_init(&livre, NULL);
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

  return 0;
}
