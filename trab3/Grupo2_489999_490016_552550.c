//----------------------------------------------
//  Trabalho 3 - MPI - Quebrando Senhas 
//  Gustavo Rodrigues Almeida       RA: 489999
//  Henrique Teruo Eihara           RA: 490016
//  Marcello da Costa Marques Acar  RA: 552550
//----------------------------------------------
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <crypt.h>
#include <mpi.h>
#include <omp.h>

// Biblioteca que possui a função clock();
#include "timer.h"

#define N_ITENS 2000
//#define TAM_SENHA 4
#define TAM_HASH 256

int encontrada = 0; 
int encontrada2 = 0;

// é definido um número padrão de threads
int NUM_THREADS = 1;

// variáveis para controle do tempo de duração da execução do programa
clock_t start = 0;
clock_t finish = 0;

int TAM_SENHA;

// vetores que guardarão a primeira senha
// e a última senha que deve ser gerada
int * vetorInicio;
int * vetorFinal;
  int my_rank;

// função que calcula a hash
void calcula_hash_senha(const char *senha, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(senha, "aa", &data));
}

// função que realiza a hash calculda e verifica
// se a hash é igual a que foi passada como parâmetro
int testa_senha(const char *hash_alvo, const char *senha) {
  char hash_calculado[TAM_HASH + 1];

  calcula_hash_senha(senha, hash_calculado);
  if (!strcmp(hash_alvo, hash_calculado)) {
    printf("%d : Senha encontrada: %s\n", my_rank, senha);
    encontrada2 = my_rank;
    return 0;
  }
  return 1;
}

int verifica(){
  if(my_rank != NUM_THREADS-1){
    MPI_Send(&encontrada,sizeof(int), MPI_INT, my_rank + 1, 1, MPI_COMM_WORLD);
  }
  else
    MPI_Send(&encontrada,sizeof(int), MPI_INT, 0, 1, MPI_COMM_WORLD);
}

// função que irá gerar as senhas e logo após
// irá verificar se a senha que foi gerada
// é a correta
void decifra(char * hash, int * vetorHibrido) {

#pragma omp parallel num_threads(2)
  if(omp_get_thread_num() == 0){
    //printf("%d : esperando mensagem de encerramento\n", my_rank);
    MPI_Recv(&encontrada, sizeof(int), MPI_INT, MPI_ANY_SOURCE,1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //printf("%d : minha thread morreu\n",my_rank);
  }else{
    // variávle que ficará armazeando a senha gerada
    char * senha = malloc(sizeof(char)*TAM_SENHA);

    // função de auxiliar para a geração de senhas
    char * senhaAux = malloc(sizeof(char)*TAM_SENHA);

    // variável que tem como significado, quando
    // deve ocorrer overflow na  cadeia de caracteres
    int overflow;

    int i;

    // cria uma string com 0
    for(i = 0; i < (TAM_SENHA); i++){
      senha[i] = '0';
    }

    // joga a senha gerada no vetorInicio (inicialmente em int)
    // para um char
    sprintf(senhaAux,"%d", vetorHibrido[0]);
    // logo após, é copiado senhaAux para variávle senha
    // em que, 'concatena' de forma que o tamanho fique
    // correto
    strcpy(&senha[TAM_SENHA - strlen(senhaAux)], senhaAux);

    // loop que ficará iterando e gerando a senha
    // a partir da string gerada anteriormente
    // apenas parara, quando for encontrada ou então
    // quando o número de senhas dimensionadas para gerar
    // a partir desta thread se esgotar
    while(!encontrada && vetorHibrido[0] <= vetorHibrido[1]){

      // loop que iterá pela string, realizando o overflow
      // quando necessário
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
      senha[TAM_SENHA] = '\0';

      // realiza o teste da senha, se for a que foi
      // passada como parâmetro, define encontrada como 1
      // sai do loop prinicpal e fecha a thread
      if(!testa_senha(hash, senha)){
        encontrada = 1;
        break;
      }

      vetorHibrido[0]++;
      overflow = 1;
    }
    verifica();
    free(senha);
    free(senhaAux);
    //printf("Thread morreu...\n");
  }
}

int main(int argc, char * argv[]) {
  int i;
  int comm_sz;

  double start, finish;

  // verifica o nÃºmero de argumentos
  MPI_Init(&argc, &argv);

  if(argc != 3){
    printf("Usage: %s <número de digitos> <hash> <número de threads>\n",argv[0]);
    exit(0);
  }

  // identifica a quantidade de processos
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  // define rank para cada processo
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  NUM_THREADS = comm_sz;
  // recebe número de threads passada como parâmetro
  TAM_SENHA = atoi(argv[1]);

  int vetorHibrido[2];
  if(my_rank == 0){
    //printf("ENTREI\n");
    // vetor que conterá as primeiras/últimas senhas que
    // devem ser geradas
    vetorInicio = malloc(sizeof(int)*NUM_THREADS);
    vetorFinal = malloc(sizeof(int)*NUM_THREADS);

    // última senha de fato ex: 9999 para uma senha de 4 digitos
    char * ultimaSenha = malloc(sizeof(char)*TAM_SENHA);
    for(i=0;i<TAM_SENHA;i++)
      ultimaSenha[i] = '9';
    ultimaSenha[i] = '\0';

    // realizando o dimensionamento das threads
    int divisao = (atoi(ultimaSenha)+1)/NUM_THREADS;

    // escolhendo a divisão de trabalho a partir da
    // ID que será criada para cada thread
    vetorInicio[0] = 0;
    vetorFinal[0] = divisao -1;
    vetorHibrido[0] = vetorInicio[0];
    vetorHibrido[1] = vetorFinal[0];
    MPI_Send(vetorHibrido, sizeof(int)*2, MPI_INT, 1, 0, MPI_COMM_WORLD);
    for(i = 1; i < NUM_THREADS; i++){
      vetorInicio[i] = vetorInicio[i-1] + divisao;
      vetorFinal[i] = vetorInicio[i] + (divisao -1);
      vetorHibrido[0] = vetorInicio[i];
      vetorHibrido[1] = vetorFinal[i];

      // mandando mensagem
      if(i < NUM_THREADS-1){
        //printf("%d : enviei mensagem\n", my_rank);
        MPI_Send(vetorHibrido, sizeof(int)*2, MPI_INT, i+1, 0, MPI_COMM_WORLD);
      }
    }
    // tratamento de caso especial, no caso, como a divisão
    // não é exata, a última thread gerará as senhas para que o número
    // fique exato
    vetorFinal[NUM_THREADS - 1] += atoi(ultimaSenha) - vetorFinal[NUM_THREADS -1];
    vetorHibrido[1] = vetorFinal[NUM_THREADS -1];

    free(ultimaSenha);
  }
  //printf("%d : esperando mensagem\n", my_rank);
  if(my_rank != 0)
    MPI_Recv(vetorHibrido, sizeof(int)*2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  //printf("%d : 0 %d\n",my_rank, vetorHibrido[0]);
  //printf("%d : 1 %d\n",my_rank, vetorHibrido[1]);

  GET_TIME(start);
  // cria as threads
  decifra(argv[2], vetorHibrido);
  GET_TIME(finish);
  if(encontrada2 == my_rank){
    printf("%d : Tempo gasto: %f\n", my_rank,(finish - start));
  }

  free(vetorInicio);
  free(vetorFinal);

  MPI_Finalize();

  // limpa o buffer e a senha
  return 0;
}
