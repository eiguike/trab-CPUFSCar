
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
char * buffer[N_ITENS];
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
  do{

    // bloqueia o buffer para que o produtor
    // possa gerar mais senhas
    pthread_mutex_lock(&bloqueio);

    // caso o buffer esteja cheio, esperara o sinal
    // quando a thread do consumidor consumir.
    // caso a chave seja encontrada, ela esperara o sinal
    // e mandara tambÃ©m um sinal para as threads consumidoras,
    // assim a produtora e as consumidoras podem fechar as threads
    while (cont == N_ITENS) {
      pthread_cond_wait(&nao_cheio, &bloqueio);
      if(encontrada){
        pthread_cond_wait(&encontrado, &bloqueio);
        pthread_cond_signal(&nao_vazio);
        pthread_mutex_unlock(&bloqueio);
        return NULL;
      }
    }

    // local aonde se gera as chaves
    auxiliar = (char*)malloc(sizeof(char)*TAM_SENHA);

    // guarda as chaves em auxiliar
    strcpy(auxiliar, senha);

    // limita o buffer para que nÃ£o ultrapasse
    // o limite decidido pelos #defines
    final = (final + 1) % N_ITENS;

    // guarda o endereÃ§o de auxiliar
    buffer[final] = auxiliar;

    // quando Ã© armazenado a nova chave
    // no contador Ã© adicionado mais um
    cont += 1;

    // manda um sinal para as threads consumidoras
    // para que elas possam consumir as chaves
    pthread_cond_signal(&nao_vazio);

    // desbloqueia o buffer
    pthread_mutex_unlock(&bloqueio);
  }while(incrementa_senha());

  // caso todas as senhas forem geradas
  // e nÃ£o derem certo a verificaÃ§Ã£o, 
  // o produtor apenas sairÃ¡
  finalizada = 1;

  // certifica que todas as senhas geradas
  // foram verificadas e espera as threads
  // consumidoras se destruirem.
  // Ã© enviado o sinal nao_vazio, pois como
  // o produtor parou de produzir senhas, o cont
  // ficarÃ¡ como zero, e no produtor, ele acabarÃ¡
  // sempre entrando no while de (cont == 0)
  do{
    pthread_cond_signal(&nao_vazio);
    pthread_mutex_unlock(&bloqueio);
  }while(finalizada < 1 + NUM_CONSUMIDOR);

  return NULL;
}

void* consumidor(void *v) {
  // variavel local para que a thread
  // possa trabalhar com ela independentemnete
  // do buffer
  char * variavelLocal = NULL;

  do{
    free(variavelLocal);

    // bloqueia o acesso de outras threads
    // no buffer
    pthread_mutex_lock(&bloqueio);

    // caso o buffer esteja vazio, esperarÃ¡ atÃ©
    // que o produtor produza algo.
    // caso jÃ¡ nÃ£o tenho mais senhas para verificar
    // e o produtor ja tiver finalizado, o consumidor
    // irÃ¡ enviar um sinal de finalizado e adicionarÃ¡
    // na variaÇ˜el global finalizada para que o produtor
    // saiba quantas threads jÃ¡ foram destruÃ­das.
    while (cont == 0) {
      pthread_cond_wait(&nao_vazio, &bloqueio);
      if(finalizada > 0){
        finalizada++;
        return NULL;
      }
    }

    // caso seja encontrada a senha, essa thread
    // enviarÃ¡ um sinal para o produtor para que
    // encerre a produÃ§Ã£o, no final, a thread
    // Ã© destruÃ­da.
    if(encontrada){
      pthread_cond_signal(&nao_cheio);
      pthread_cond_signal(&encontrado);
      pthread_mutex_unlock(&bloqueio);
      finalizada++;
      return NULL;
    }

    // permite apenas o acesso do buffer
    // limitado pelo #define
    inicio = (inicio + 1) % N_ITENS;
    variavelLocal = buffer[inicio];

    // diminui o nÃºmero de chaves armazenadas
    // no buffer
    cont -= 1;

    // desbloqueia para que produtor crie mais
    // chaves
    pthread_cond_signal(&nao_cheio);
    pthread_mutex_unlock(&bloqueio);
  }while(testa_senha(senhaAlvo, variavelLocal));

  // caso encontre a chave, Ã© definido em uma variÃ¡vle
  // global encontrada = 1, para que as outras threads
  // possam ter conhecimento
  encontrada = 1;
      pthread_cond_signal(&nao_cheio);
      pthread_cond_signal(&encontrado);
      pthread_mutex_unlock(&bloqueio);
 
  finalizada++;
  return NULL;
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
