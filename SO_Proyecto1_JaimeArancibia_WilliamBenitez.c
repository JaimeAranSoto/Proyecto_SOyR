#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define dimx 100
#define dimy 100

typedef struct {
	int x, y;
	int mat[dimx][dimy];
}datos;

void *functioncontador1(void *i);
void *functioncontador2(void *i);


void llenarMatriz(void *i){
	datos *v = (datos *)i;
	int k, j;
	for (k = 0; k < v->x; k++) {
		for (j = 0; j < v->y; j++) {
			v->mat[k][j] = 1 + rand() % 255;
		}
	}
}

void main()
{
	int i, j;
	pthread_t thread1, thread2;
	datos * d = (datos *)malloc(sizeof(datos));

	d->x = 10;
	d->y = 20;
	llenarMatriz((void*)d);

	pthread_create(&thread1, NULL, &functioncontador1, (void *)d);
	pthread_create(&thread2, NULL, &functioncontador2, (void *)d);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	printf("Dim x:%d\n", d->x);
	printf("Dim y:%d\n", d->y);
	printf("\n");

	
	for (i = 0; i < d->x; i++) {
		for (j = 0; j < d->y; j++) {
			printf("%d ", d->mat[i][j]);
		}
		printf("\n");
	}
	exit(EXIT_SUCCESS);
}
void *functioncontador1(void *i) {
	printf("Estoy en el thread 1...\n");
	
	printf("Terminé thread 1...\n");
}
void *functioncontador2(void *d) {
	printf("Estoy en el thread 2...\n");
	
	printf("Terminé el thread 2...\n");
}


