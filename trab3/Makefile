CC=mpicc

all:
	$(CC) Grupo2_489999_490016_552550.c -fopenmp -lcrypt -o quebraSenhas
	gcc gen_hash.c -o gen_hash -lcrypt
clean:
	rm quebraSenhas gen_hash
