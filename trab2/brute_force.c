#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <crypt.h>
#include <stdlib.h>
#include <time.h>

#include <omp.h>

#define TAM_HASH 256
#define THREADS_PRODUTOR 

void CalculaHashSenha(const char *senha, char *hash);
int IncrementaSenha(char *senha);
int TestaSenha(const char *hash_alvo, const char *senha);

int tamanhoSenha = 0;
char *senha;

void produtor(){
	# pragma omp parallel num_threads(THREADS_PRODUTOR)

}

void consumidor(){

}


int main(int argc, char *argv[])
{
	int achou = 0;
	
	clock_t start = 0;
	clock_t finish = 0;

	if(argc < 3)
	{
		printf("Uso: %s <tamanho da senha> <hash>", argv[0]);
		return 1;
	}
	tamanhoSenha = atoi(argv[1]);
	senha = malloc(sizeof(char) * (tamanhoSenha + 1));
	memset(senha, '0', tamanhoSenha);
	senha[tamanhoSenha] = '\0';

	start = clock();

	while ((achou=TestaSenha(argv[2], senha)) == 0 && IncrementaSenha(senha) != -1);
	
	finish = clock();

	if(achou)
	{
		printf("%f\n",(float)(finish - start)/CLOCKS_PER_SEC);
	}

	return 0;
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

int IncrementaSenha(char *senha)
{
	int index = tamanhoSenha - 1;

	while (index >= 0)
	{
		if (senha[index] != '9')
		{
			senha[index]++;
			
			index = -2;
		}
		else
		{
			senha[index] = '0';
		}
		
		index--;
	}
	
	return index;
}

void CalculaHashSenha(const char *senha, char *hash) {
	struct crypt_data data;
	data.initialized = 0;

	strcpy(hash, crypt_r(senha, "aa", &data));
}
