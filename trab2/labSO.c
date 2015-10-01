
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

int divisao;
static int NUM_THREADS = NUM_PRODUTOR + NUM_CONSUMIDOR;

char * senhaAlvo;
char * auxiliar;

// variáveis para controle do tempo de duração da execução do programa
clock_t start = 0;
clock_t finish = 0;

int TAM_SENHA;

int inicio = 0, final = 0, cont = 0, encontrada = 0, finalizada = 0;
int produtorNumeros = 0;
int consumidorNumeros = 0;
int * comecoThread;

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

if(strcmp(senha,"8147")==0)
//	printf("HEUHEUHE ID: %d\n",omp_get_thread_num());
//printf("gerei a senha %s ID: %d\n",senha, omp_get_thread_num());

  //printf("senha gerada %s ID: %d\n",senha, omp_get_thread_num());
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

  //printf("testei a senha %s ID: %d\n", senha, omp_get_thread_num());

  calcula_hash_senha(senha, hash_calculado);
//	printf("consumi a senha %s\n",senha);
  if (!strcmp(hash_alvo, hash_calculado)) {
    printf("ACHEI A SENHA!!!! %s\n", senha);
    return 0;
  }
  //printf("%s %s %s\n",hash_alvo, hash_calculado, senha);
  return 1;
}

void produtor(int rank) {
  // vetor de senha que será iterada
  // neste escopo
  char ** variavelLocal = NULL;
  int i;

  // variável que determina se não há mais
  // senha para gerar nesta thread
  int finalizado = 1;

  // número de senhas que ja foram geradas
  int acumuladorSenhasGeradas = 0;

  // senha individual, que será adicionada na variavelLocal
  char * senhaLocal = (char*) malloc(sizeof(char)*TAM_SENHA);
  char * aux = (char*) malloc(sizeof(char)*TAM_SENHA);

  // gera a primeira senha do produtor
  if(comecoThread[omp_get_thread_num()] == 0){
    for(i=0;i<TAM_SENHA;i++){
      senhaLocal[i] = '0';
    }
  }else{
    // caso o produtor não seja a thread 0, gerara
    // a senha apartir da conta feita no main
    sprintf(aux, "%d", comecoThread[rank]);
    int conta = strlen(aux) - TAM_SENHA;
    for(i=0;i<conta;i++)
      senhaLocal[i] = '0';
    strcat(senhaLocal,aux);
  }
	
	printf("divisao: %d minha primeir asenha é: %s tenho id: %d\n",divisao,senhaLocal, omp_get_thread_num());

  // aqui começa a mágica
  do{
    variavelLocal = (char**) malloc(sizeof(char*)*N_ITENS);
    for(i=0;i<N_ITENS;i++){
      variavelLocal[i] = NULL;
    }
    i = 0;
    // loop que acontece a criação de senhas
    do{
      // em caso de algumas threads consumidoras encontrem
      // alguma senha
      if(!finalizado || encontrada){
        break;
      }
      // local aonde se gera as chaves
      auxiliar = (char*)malloc(sizeof(char)*TAM_SENHA);
      strcpy(auxiliar, senhaLocal);
      variavelLocal[i] = auxiliar;
      i++;
      acumuladorSenhasGeradas++;

      // esse loop só parara quando,
      // não houver mais senhas para gerar
      // quando o número de senhas geradas já ultrapassou o N_ITENS
    }while((i <= N_ITENS)&&(finalizado = incrementa_senha(senhaLocal)) && acumuladorSenhasGeradas < divisao);
	
	if(acumuladorSenhasGeradas >= divisao)
		printf("divisao %d minha ultima senha: %s ID: %d\n",divisao,auxiliar,omp_get_thread_num());

    // sleep mto provavelmente  desnecessário
    sleep(3);

    // parte qe também acho desnecessário, entretanto
    // utilizo para não utilizar toda memora do computador
    int valor;
    while(1){
#pragma omp critical(fila)
      valor = adicionaFila(variavelLocal);	
	
      if(valor || encontrada){
        break;
      }
    }


    // quando é encontrado a senha, ou então, é finalizado a gearção de senhas
    // ou então, o acmulador de senhas geradas já ultrapassou o limite
    // é finalizado a tread
    if(!finalizado || encontrada || acumuladorSenhasGeradas >= divisao){
      break;
    }

  }while(1);

  // só soma com o número de threads que sairam
  produtorNumeros++;
  printf("Produtor morreu...\n");
}

void consumidor() {
  // variavel local para que a thread
  // possa trabalhar com ela independentemnete
  // do buffer
  char ** variavelLocal2 = NULL;
  int i;

  do{
    // como openmp não tem um tratador condicional que deixa a thread
    // em standby, foi necessário esse if
    if(head != NULL){
#pragma omp critical(fila)
      variavelLocal2 = removerFila();

      for(i=0; (i < N_ITENS) && (variavelLocal2 != NULL); i++){
        if(variavelLocal2[i] != NULL)
          if(!testa_senha(senhaAlvo, variavelLocal2[i])){
            encontrada = 1;
            finalizada++;
            free(variavelLocal2[i]);
          }else{
            free(variavelLocal2[i]);
          }
      }
      free(variavelLocal2);

      if(encontrada)
        break;
    }
    
    // if utilizado para verificar se todos os produtores
    // ja sairam, se sim, o consumidro sai tbm.
    if(produtorNumeros == NUM_PRODUTOR)
      break;
  }while(1);

  printf("Consumidor morreu...\n");
}

int main(int argc, char *argv[]) {
  int i;
  char * senha;

  // prepara a fila
  criarFila();

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

  // inicia a contagem
  start = clock();

  // divido o trabalho dos produtores
  divisao = atoi("999999");
  divisao = divisao/NUM_PRODUTOR;	

  comecoThread = malloc(sizeof(int)*(NUM_PRODUTOR));
  comecoThread[0] = 0;
  for( i = 1; i < NUM_PRODUTOR; i++){
    comecoThread[i] = comecoThread[i-1] + divisao;
  }

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
  free(senhaAlvo);

  return 0;
}

