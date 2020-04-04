#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

#define NTHREADS 2

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

char** shared_data;

void *some_thread1(void *workID){
	pthread_mutex_lock(&mutex1);
	printf("Working in Thread1\n");
	char* A = (char*)malloc(8);
	A[0] = 'A';
	A[1] = '\0';
	char* B = (char*)malloc(8);
	B[0] = 'B';
	B[1] = '\0';
	char* C = (char*)malloc(8);
	C[0] = 'C';
	C[1] = '\0';
	char* D = (char*)malloc(8);
	D[0] = 'D';
	D[1] = '\0';
	char* E = (char*)malloc(8);
	E[0] = 'E';
	E[1] = '\0';
	char* F = (char*)malloc(8);
	F[0] = 'F';
	F[1] = '\0';
	char* G = (char*)malloc(8);
	G[0] = 'G';
	G[1] = '\0';
	char* H = (char*)malloc(8);
	H[0] = 'H';
	H[1] = '\0';
	shared_data[0] = A;
	shared_data[1] = B;
	shared_data[2] = C;
	shared_data[3] = D;
	shared_data[4] = E;
	shared_data[5] = F;
	shared_data[6] = G;
	shared_data[7] = H;
	
	pthread_mutex_unlock(&mutex1);
	return NULL;
}

void *some_thread2(void *workID){
	
	pthread_mutex_lock(&mutex1);
	printf("Working in Thread2\n");	
	char* I = (char*)malloc(8);
	I[0] = 'I';
	I[1] = '\0';
	char* J = (char*)malloc(8);
	J[0] = 'J';
	J[1] = '\0';
	char* K = (char*)malloc(8);
	K[0] = 'K';
	K[1] = '\0';
	char* L = (char*)malloc(8);
	L[0] = 'L';
	L[1] = '\0';
	char* M = (char*)malloc(8);
	M[0] = 'M';
	M[1] = '\0';
	char* N = (char*)malloc(8);
	N[0] = 'N';
	N[1] = '\0';
	char* O = (char*)malloc(8);
	O[0] = 'O';
	O[1] = '\0';
	char* P = (char*)malloc(8);
	P[0] = 'P';
	P[1] = '\0';
	shared_data[8] = I;
	shared_data[9] = J;
	shared_data[10] = K;
	shared_data[11] = L;
	shared_data[12] = M;
	shared_data[13] = N;
	shared_data[14] = O;
	shared_data[15] = P;

	pthread_mutex_unlock(&mutex1);
	return NULL;
}

// Here is some data structure that will be shared.
// It is an array of strings, but nothing has been allocated yet.


int main(){
	// (1) Malloc for some size of your shared data (i.e. how many strings will you have)
	shared_data = (char**)malloc(16 * sizeof(char*));
	// (2) Launch some number of threads (perhaps with two or more different functions)
	pthread_t tids[NTHREADS];
	pthread_create(&tids[0], NULL, some_thread1, NULL);
	pthread_create(&tids[1], NULL, some_thread2, NULL);
	// (3) Join some number of threads	
	pthread_join(tids[0], NULL);
	pthread_join(tids[1], NULL);
	// (4) Print the results of shared data (i.e. this is done sequentially)
	int i;
	for (i = 0; i < 16; i++) {
		printf("%s\n", shared_data[i]);
	}
	// (5) Cleanup your program
	for (i = 0; i < 16; i++) {
		free(shared_data[i]);
	}
	free(shared_data);
}
