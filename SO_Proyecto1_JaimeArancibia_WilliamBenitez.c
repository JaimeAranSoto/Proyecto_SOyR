#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define dimx 100
#define dimy 100
#define maxValue 255

typedef struct
{
	int x, y;
	int mat[dimx][dimy];
	int part;
} datos;

pthread_mutex_t contador_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condicion_var = PTHREAD_COND_INITIALIZER;

void *functionThread(void *i);
void imprimirMatriz(datos *d);
void llenarMatriz(datos *v);
int sumaMatriz(datos *d, int min, int max);

int suma = 0;
int cantidadThreads =2;

void main()
{
	int i, j;
	pthread_t threads[cantidadThreads];

	datos *d = (datos *)malloc(sizeof(datos));

	//dimensiones matriz
	d->x = 6;
	d->y = 6;

	llenarMatriz(d);


	d->part = 0;
	for(i=0;i<cantidadThreads;i++){
	pthread_create(&threads[i], NULL, &functionThread, (void *)d);
	}

	for(i=0;i<cantidadThreads;i++){
	pthread_join(threads[i], NULL);
	}

	printf("Dim x:%d\n", d->x);
	printf("Dim y:%d\n", d->y);
	printf("\n");

	int promedio = suma/(d->x*d->y);
	printf("El promedio es %d...\n",promedio);

	imprimirMatriz(d);

	exit(EXIT_SUCCESS);
}

void imprimirMatriz(datos *d)
{
	int i, j;
	for (i = 0; i < d->x; i++)
	{
		for (j = 0; j < d->y; j++)
		{
			printf("%d ", d->mat[i][j]);
		}
		printf("\n");
	}
}

void *functionThread(void *i)
{
	pthread_mutex_lock(&contador_mutex);
	datos *v = (datos *)i;
	printf("-Estoy en el thread %d...\n", v->part);
	int suma_ = sumaMatriz(v,v->part*(v->x*v->y)/cantidadThreads,((v->part+1)*(v->x*v->y)/cantidadThreads)-1);
	printf("Mi suma es %d\n",suma_);
	v->part++;
	suma+=suma_;
	printf("Terminé thread %d\n", v->part - 1);

	pthread_mutex_unlock(&contador_mutex);
}

void llenarMatriz(datos *v)
{

	int k, j;
	for (k = 0; k < v->x; k++)
	{
		for (j = 0; j < v->y; j++)
		{
			v->mat[k][j] = 1 + rand() % maxValue;
		}
	}
}

int sumaMatriz(datos *d, int min, int max)
{
	int suma = 0;
	int k, j;
	
	for (k = min; k <= max; k++)
	{
		int fila = k/d->y;
		int col = k - (fila*d->y);
		suma+=d->mat[fila][col];
	}
	return suma;
}
