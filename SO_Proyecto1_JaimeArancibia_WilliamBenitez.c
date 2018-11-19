#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define maxHeight 1000
#define maxWidth 1000
#define maxValue 255 //Valor máximo que puede tomar un valor de la matriz

//Definición de estructura
typedef struct
{
	int height, width; //Dimesiones de la matriz
	int mat[maxHeight][maxWidth];
} datos;

//Estructura que permite pasar la matriz ("global") junto con un dato entero (el índice desde el cual partir la suma)
typedef struct
{
	datos *d;
	int part;
} mensaje;

pthread_mutex_t contador_mutex = PTHREAD_MUTEX_INITIALIZER;

//Definición de funciones
void *functionThread(void *i);
void imprimirMatriz(datos *d);
void llenarMatriz(datos *v);
int sumaMatriz(datos *d, int min, int max);

int suma = 0;
int cantidadThreads = 2;

void main()
{
	int i; //Variables para loop
	pthread_t threads[cantidadThreads];

	datos *d = (datos *)malloc(sizeof(datos)); //Creación datos

	//Dimensiones matriz TODO: Leer desde la consola
	d->height = 50;
	d->width = 50;

	llenarMatriz(d);
	printf("Height:%d\n", d->height);
	printf("Width:%d\n", d->width);
	printf("\n");

	/*Thread*/

	for (i = 0; i < cantidadThreads; i++)
	{
		mensaje *m = (mensaje *)malloc(sizeof(mensaje));
		m->d = d;
		m->part = i; //Asigna a la variable part en cuál hilo estamos (por orden de creación)
		pthread_create(&threads[i], NULL, &functionThread, (void *)m);
	}

	for (i = 0; i < cantidadThreads; i++)
	{
		pthread_join(threads[i], NULL); //Espera a que terminen todos los hilos
	}

	int promedio = suma / (d->height * d->width);
	printf("El promedio es %d...\n", promedio);

	/*Fin Thread*/

	imprimirMatriz(d);

	exit(EXIT_SUCCESS);
}

void imprimirMatriz(datos *d)
{
	int i, j;
	for (i = 0; i < d->height; i++)
	{
		for (j = 0; j < d->width; j++)
		{
			printf("%d ", d->mat[i][j]);
		}
		printf("\n");
	}
}

void *functionThread(void *m)
{
	mensaje *v = (mensaje *)m;
	int part = v->part;
	printf("-Estoy en el thread %d...\n", part);
	int suma_ = sumaMatriz(v->d, part * (v->d->height * v->d->width) / cantidadThreads, ((part + 1) * (v->d->height * v->d->width) / cantidadThreads) - 1);
	printf("Mi suma es %d\n", suma_);
	pthread_mutex_lock(&contador_mutex); //Bloquea el hilo para manejar datos (concurrencia)
	suma += suma_;						 //Agrega la suma parcial al total global
	pthread_mutex_unlock(&contador_mutex);
	printf("Terminé thread %d...\n", part);
}

void llenarMatriz(datos *v)
{
	//Rellena la matriz con número enteros entre 1 y maxValue
	int k, j;
	for (k = 0; k < v->height; k++)
	{
		for (j = 0; j < v->width; j++)
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
		int fila = k / d->width;
		int col = k - (fila * d->width);
		suma += d->mat[fila][col];
	}
	return suma;
}
