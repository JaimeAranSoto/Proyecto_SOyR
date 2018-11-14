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


void main()
{
	int i, j;
	pthread_t thread1, thread2;
	datos * d = (datos *)malloc(sizeof(datos));
	
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
	printf("Estoy en el proceso 1...\n");
	//printf("\nContador 1\n");
	datos *v = (datos *)i;
	v->x = 10;
	v->y = 20;
	printf("Terminé proceso 1...\n");
}
void *functioncontador2(void *d) {
	printf("Estoy en el proceso 2...\n");
	//printf("\nContador 2\n");
	datos *v = (datos *)d;
	sleep(1);
	int i, j;
	for (i = 0; i < v->x; i++) {
		for (j = 0; j < v->y; j++) {
			v->mat[i][j] = i;
		}
	}
	printf("Terminé el proceso 2...\n");
}
