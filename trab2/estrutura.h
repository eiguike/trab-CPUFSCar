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
/*
	fila aux = (fila*)malloc(sizeof(fila)*1);
	aux->senhas = senhas;
	aux = head;
	aux->prox = NULL;
	aux->prox = NULL;
	if(head == NULL){
		head = aux;
		tail = aux;
	}else{
		tail->prox = aux;
		tail = aux;
	}
*/
}

void criarFila(){
	head = NULL;
	tail = NULL;
}

int * removerFila(){
	
}


