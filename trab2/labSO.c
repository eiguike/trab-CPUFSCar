#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <crypt.h>
#include <omp.h>
#include <time.h>

#define N_ITENS 100 
#define TAM_HASH 256

int TAM_SENHA;
int NUM_THREADS;
char * senhaAlvo;
char * buffer[N_ITENS];
char * senha;
char * auxiliar;

omp_lock_t lockProdutor;

int inicio = 0, final = 0, cont = 0, encontrada = 1, 
produtorFinalizado = 0,
consumidorFinalizado = 0;

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

  // printf("%d: GEREI A SENHA %s\n", omp_get_thread_num(),senha);
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

  // printf("%s %s\n",hash_alvo,hash_calculado);
  // printf("%d: CONSUMI A SENHA %s\n",omp_get_thread_num(),senha);
  if (strcmp(hash_alvo, hash_calculado) == 0) {
    return 0;
  }
  return 1;
}

void produtor() {
  do{
    // o produtor devera ficar parado
    while (cont >= (N_ITENS-1)){
      //printf("%d: ESTA CHEIO\n", omp_get_thread_num());
      if(!encontrada || (produtorFinalizado == (NUM_THREADS/2))){
        // free(auxiliar);
        return;
      }
    }

    omp_set_lock(&lockProdutor);
    // printf("Produtor %d : TRAVEI\n",omp_get_thread_num());
    auxiliar = (char*)malloc(sizeof(char)*(TAM_SENHA+2));
    strcpy(auxiliar, senha);
    buffer[final] = auxiliar;
    //printf("%d: COLOQUEI NO BUFFER %d %d SENHA %s\n",omp_get_thread_num(),final, (final+1)%N_ITENS, auxiliar);
    final = (final + 1) % N_ITENS;
    cont += 1;
    // printf("Produtor %d : DESTRAVEI\n",omp_get_thread_num());
    omp_unset_lock(&lockProdutor);
  

  } while(incrementa_senha());
  produtorFinalizado++;

  printf("%d: Produtor morreu\n",omp_get_thread_num());
  return ;
}

void consumidor() {
  char * variavelLocal = NULL;

  do{
    free(variavelLocal);
    while(cont <= 0){
      // printf("heheh %d\n",encontrada);
      if(!encontrada || (produtorFinalizado == (NUM_THREADS/2) ) || (consumidorFinalizado > 0) ){
        consumidorFinalizado++;
        printf("Consumidor %d: MORRI\n",omp_get_thread_num());
        return;
      }
    }

    omp_set_lock(&lockProdutor);
    // printf("Consumidor %d : TRAVEI\n",omp_get_thread_num());


    variavelLocal = buffer[inicio];
    buffer[inicio] = NULL;

    inicio = (inicio + 1) % N_ITENS;
    cont -= 1;
    // printf("Consumidor %d : DESTRAVEI\n",omp_get_thread_num());
    omp_unset_lock(&lockProdutor);

    encontrada = testa_senha(senhaAlvo, variavelLocal);
  }while(encontrada);
  free(variavelLocal);

  consumidorFinalizado++;
  printf("Consumidor %d: Senha encontrada\n", omp_get_thread_num());

  return ;
}

int main(int argc, char *argv[]) {
  int i;
  clock_t start = 0;
  clock_t finish = 0;

  // verifica o nÃºmero de argumentos
  if(argc != 4){
    printf("Usage: %s <tamanho da senha> <hash> <threads>\n",argv[0]);
    exit(0);
  }

  NUM_THREADS = atoi(argv[3]);

  // senha hash
  senhaAlvo = (char*)malloc(sizeof(char)*(strlen(argv[2])+1));
  strcpy(senhaAlvo,argv[2]);

  TAM_SENHA = atoi(argv[1]);
  senha = (char*) malloc(sizeof(char)*(TAM_SENHA+1));
  for(i=0;i<TAM_SENHA; i++){
    senha[i] = '0';
  }
  senha[TAM_SENHA] = '\0';
  omp_init_lock(&lockProdutor);
  start = clock();

  #pragma omp parallel num_threads (NUM_THREADS)
  if(omp_get_thread_num() < NUM_THREADS/2){
    produtor();
  }else{
    consumidor();
  }
  finish = clock();
  omp_destroy_lock(&lockProdutor);

  printf("%f\n",(float)(finish - start)/CLOCKS_PER_SEC);
    for(i=0;i<N_ITENS;i++)
    free(buffer[i]);
  // limpa o buffer e a senha
  free(senhaAlvo);
  free(senha);

  return 0;
}