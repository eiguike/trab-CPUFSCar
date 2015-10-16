#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _fila{
	struct _fila * prox;
	char ** senhas;
  int hue;
}fila;

fila * tail;
fila * head;

// defini um tamanho máixmo para não
// utilizar toda a memória do computador
// pode ser um limitante isso daqui...
int tamanhoFila = 0;
int TAM_MAXFILA = 10000;
int counter = 0;

void printarFila(){
  fila * aux = head;
  while(aux != NULL){
    printf("%d contador\t", aux->hue);
    aux = aux->prox;
  }
  printf("\n");

}
int adicionaFila(char ** senhas){
  if(tamanhoFila >= TAM_MAXFILA)
    return 0;
  else{
    counter++;
    fila * aux = (fila*)malloc(sizeof(fila)*1);
    aux->hue = counter;
    aux->senhas = senhas;
    aux->prox = NULL;
    if(head == NULL){
      head = aux;
      tail = aux;
    }else{
      tail->prox = aux;
      tail = aux;
    }
    tamanhoFila++;

    return 1;
  }
}

void criarFila(){
	head = NULL;
	tail = NULL;
}

char ** removerFila(){
  fila * aux;
  char** auxSenha;

  if(head == NULL)
    return NULL;

  auxSenha = head->senhas;
  aux = head->prox;
  free(head);
  head = aux;
  tamanhoFila--;

  return auxSenha;
}


