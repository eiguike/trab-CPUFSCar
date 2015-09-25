
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
#define NUM_THREADS 4

char * senhaAlvo;
char ** buffer;
char ** buffer2;
char * senha;
char * auxiliar;

// variáveis para controle do tempo de duração da execução do programa
clock_t start = 0;
clock_t finish = 0;

int TAM_SENHA;

int inicio = 0, final = 0, cont = 0, encontrada = 0, finalizada = 0;
int produtorNumeros = 0;
int consumidorNumeros = 0;
int * comecoThread;

int incrementa_senha_conformeItens(){
	int proxSenha = strtol(senha, NULL,10);
	printf("Proxima senha: %d", proxSenha);
}

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
	//printf("%s %s %s\n",hash_alvo, hash_calculado, senha);
	return 1;
}

void produtor(int rank) {
	char ** variavelLocal = NULL;
	int i;
	int finalizado = 1;
	char * senhaLocal = (char*) malloc(sizeof(char)*TAM_SENHA);
	char * aux = (char*) malloc(sizeof(char)*TAM_SENHA);

	// gera senha inicial do produtor
	printf("EHUHEUH: %d\n", rank%(NUM_THREADS/2));
	printf("COMECOOOO THREAD %d MEU RANK É %d\n", comecoThread[rank%(NUM_THREADS/2)],rank);	
	if(comecoThread[omp_get_thread_num()] == 0){
		for(i=0;i<TAM_SENHA;i++){
			senhaLocal[i] = '0';
		}
	}else{
		printf("hue\n");
		sprintf(aux, "%d", comecoThread[rank]);
		int conta = strlen(aux) - TAM_SENHA;
		for(i=0;i<conta;i++)
			senha[i] = '0';
		strcat(senhaLocal,aux);
	}

	printf("MINHA PRIMEIRA SENHA É: %s SOU O %d",senhaLocal, rank);
/*	
	do{
		if(encontrada && !finalizada){
			break;
		}
		
		variavelLocal = (char**) malloc(sizeof(char*)*N_ITENS);
		for(i=0;i<N_ITENS;i++){
			variavelLocal[i] = NULL;
		}
		i = 0;
		do{
			if(!finalizado){
				break;
			}
			// local aonde se gera as chaves
			auxiliar = (char*)malloc(sizeof(char)*TAM_SENHA);
			strcpy(auxiliar, senha);
			variavelLocal[i] = auxiliar;
			i++;
		}while((i < N_ITENS)&&(finalizado = incrementa_senha()));

		if(encontrada && !finalizado)
			break;

		// ACESSO ATÔMICO PARA GERAÇÃO DE SENHAS
		pthread_mutex_lock(&bloqueio);
		buffer = variavelLocal;
		pthread_cond_signal(&nao_vazio);
		pthread_cond_wait(&nao_cheio, &bloqueio);

		if(encontrada && !finalizado)
			break;
		pthread_mutex_unlock(&bloqueio);
	}while(1);

	produtorNumeros++;
	while(consumidorNumeros < NUM_CONSUMIDOR){
		pthread_cond_signal(&nao_vazio);
		pthread_mutex_unlock(&bloqueio);
	}
*/

	printf("Produtor morreu...\n");
}

void consumidor() {
/*	// variavel local para que a thread
	// possa trabalhar com ela independentemnete
	// do buffer
	char ** variavelLocal2 = NULL;
	int i;

	do{
		if(encontrada){
			finalizada++;
			break;
		}

		// LEMBRETE, VERIFICAR SE O BUFFER É NULL
		pthread_mutex_lock(&bloqueio);
		variavelLocal2 = buffer;
		buffer = NULL;

		pthread_cond_signal(&nao_cheio);
		pthread_cond_wait(&nao_vazio, &bloqueio);
		pthread_mutex_unlock(&bloqueio);

		for(i=0; (i < N_ITENS) && (variavelLocal2 != NULL); i++){
			if(variavelLocal2[i] != NULL)
				if(!testa_senha(senhaAlvo, variavelLocal2[i])){
					encontrada = 1;
					finalizada++;
					pthread_cond_signal(&nao_cheio);
				}else{
					free(variavelLocal2[i]);
				}
		}

		if(encontrada)
			break;
		free(variavelLocal2);
	}while(1);

	consumidorNumeros++;

	while(produtorNumeros <  NUM_PRODUTOR){
		pthread_cond_signal(&nao_cheio);
		pthread_mutex_unlock(&bloqueio);
	}
*/
	printf("Consumidor morreu...\n");
}

int main(int argc, char *argv[]) {
	int i;

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

	// inicia a contagem
	start = clock();

	// divido o trabalho dos produtores
	int divisao = atoi("9999");
	divisao = divisao/(NUM_THREADS/2);	
	
	comecoThread = malloc(sizeof(int)*(NUM_THREADS/2));
	comecoThread[0] = 0;
	for( i = 1; i < NUM_THREADS/2; i++){
		comecoThread[i] = comecoThread[i-1] + divisao + 1;
		printf("%d comecothread\n", comecoThread[i]);
	}

	# pragma omp parallel num_threads(NUM_THREADS)
	if((omp_get_thread_num() % 2) == 0){
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
	free(*buffer);

	return 0;
}
