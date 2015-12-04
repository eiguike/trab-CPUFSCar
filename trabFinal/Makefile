CC=gcc

all:
	$(CC) EfeitoParticula.c -o paralelo -lGL -lGLU -lglut -lm -lpthread
	$(CC) serializado.c -o serial -lGL -lGLU -lglut -lm
clean:
	rm paralelo 
	rm serial

