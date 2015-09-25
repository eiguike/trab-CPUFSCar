#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _fila{
	struct _fila * prox;
	int * senhas;
}fila; 

fila * tail;
fila * head;

void adicionaFila(int * senhas){
	fila aux = (fila*)malloc(sizeof(fila)*1);
	aux->senhas = senhas;
	aux = head;
	aux->prox = NULL;
	aux->prox = NULL;
	if(tail == NULL){
		tail->prox 	
	}
}

void criarFila(){
	head = NULL;
	tail = NULL;
}

fila * removerFila(){

}


