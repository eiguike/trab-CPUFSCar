CC=gcc

all: gen_hash brute_force

gen_hash: gen_hash.o
	$(CC) -o gen_hash gen_hash.c -lcrypt

brute_force: brute_force.o
	$(CC) -o brute_force brute_force.c -lcrypt
