#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
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
void clasificar(float avg);

int cantidadThreads = 2;
int *suma;
void main()
{
	int i;		 //Variables para loop
	long T1, T2; //Variables para cálculo de tiempo
	float delta = 0;

	pthread_t threads[cantidadThreads];

	datos *d = (datos *)malloc(sizeof(datos)); //Creación datos
	suma = (int *)malloc(sizeof(int));
	*suma = 0;
	printf("El valor de la suma es: %d\n", *suma);

	//Dimensiones matriz
	printf("matrix_rows: ");
	scanf("%d", &d->height);
	printf("matrix_columns: ");
	scanf("%d", &d->width);
	printf("\n");

	llenarMatriz(d);
	printf("Height:%d\n", d->height);
	printf("Width:%d\n", d->width);
	printf("\n");

	/*Thread*/
	T1 = clock();
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

	float promedio = (float)*suma / (d->height * d->width);
	printf("El promedio es %f...\n", promedio);
	clasificar(promedio);
	T2 = clock();
	delta = (float)(T2 - T1) / CLOCKS_PER_SEC;

	printf("\nTime: %.5f segundos\n", delta);
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
	*suma += suma_;						 //Agrega la suma parcial al total global
	pthread_mutex_unlock(&contador_mutex);
	printf("Terminé thread %d...\n", part);
}

void llenarMatriz(datos *d)
{
	//Rellena la matriz con número enteros entre 1 y maxValue
	int i, j;
	for (i = 0; i < d->height; i++)
	{
		for (j = 0; j < d->width; j++)
		{
			d->mat[i][j] = 1 + rand() % maxValue;
		}
	}
}

int sumaMatriz(datos *d, int min, int max)
{

	int suma = 0;
	int i, j;
	//Recorre la matriz como si fuera un array unidimensional, desde un índice mínimo a uno máximo.
	for (i = min; i <= max; i++)
	{
		int fila = i / d->width;
		int col = i - (fila * d->width);
		suma += d->mat[fila][col];
	}
	return suma;
}

void clasificar(float avg)
{

	if (avg <= 127)
		printf("\nMatriz Oscura\n");
	else
		printf("\nMatriz Clara\n");
}
