#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main(){
	# pragma omp parallel num_threads(4)
	printf("meu rank é o %d\n", omp_get_thread_num());
}
