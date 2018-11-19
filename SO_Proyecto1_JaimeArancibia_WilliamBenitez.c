#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define dimx 100
#define dimy 100
#define maxValue 255 //Valor máximo que puede tomar un valor de la matriz

//Definición de estructura
typedef struct
{
	int x, y; //Dimesiones de la matriz
	int mat[dimx][dimy];
} datos;

typedef struct
{
	datos *d;
	int part;
} mensaje;

pthread_mutex_t contador_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condicion_var = PTHREAD_COND_INITIALIZER;

//Definición de funciones
void *functionThread(void *i);
void imprimirMatriz(datos *d);
void llenarMatriz(datos *v);
int sumaMatriz(datos *d, int min, int max);

int suma = 0;
int cantidadThreads = 2;

void main()
{
	int i, j;
	pthread_t threads[cantidadThreads];

	datos *d = (datos *)malloc(sizeof(datos)); //Creación datos

	//Dimensiones matriz TODO: Leer desde la consola
	d->x = 50;
	d->y = 50;

	llenarMatriz(d);

	/*Thread*/

	for (i = 0; i < cantidadThreads; i++)
	{
		mensaje *m = (mensaje *)malloc(sizeof(mensaje));
		m->d = d;
		m->part = i;
		pthread_create(&threads[i], NULL, &functionThread, (void *)m);
	}

	for (i = 0; i < cantidadThreads; i++)
	{
		pthread_join(threads[i], NULL);
	}

	printf("Dim x:%d\n", d->x);
	printf("Dim y:%d\n", d->y);
	printf("\n");

	int promedio = suma / (d->x * d->y);
	printf("El promedio es %d...\n", promedio);

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
	mensaje *v = (mensaje *)i;
	int part = v->part; //TODO sacar del mutex, o no será realmente paralelización
	printf("-Estoy en el thread %d...\n", part);
	int suma_ = sumaMatriz(v->d, part * (v->d->x * v->d->y) / cantidadThreads, ((part + 1) * (v->d->x * v->d->y) / cantidadThreads) - 1);
	printf("Mi suma es %d\n", suma_);
	pthread_mutex_lock(&contador_mutex);
	suma += suma_;
	pthread_mutex_unlock(&contador_mutex);
	printf("Terminé thread %d\n", part);
}

void llenarMatriz(datos *v)
{
	//Rellena la matriz con número enteros entre 1 y maxValue
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
	//Recorre la matriz como si fuera un array unidimensional, desde un índice mínimo a uno máximo.
	for (k = min; k <= max; k++)
	{
		int fila = k / d->y;
		int col = k - (fila * d->y);
		suma += d->mat[fila][col];
	}
	return suma;
}
