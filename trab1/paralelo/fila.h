typedef struct fila{
  fila * prox;
  char * senha;
}Fila;

int addFila(Fila * fila){
  Fila * aux = fila;

  while(aux->prox != NULL){
    aux = aux->prox
  }

}
