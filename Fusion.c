#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
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
} matrixData;

//Estructura que permite pasar la matriz ("global") junto con un dato entero (el índice desde el cual partir la suma)
typedef struct
{
	matrixData *d;
	int part;
} message;

pthread_mutex_t mutex_counter = PTHREAD_MUTEX_INITIALIZER;

//Definición de funciones
void *functionThread(void *i);
void printMatrix(matrixData *d);
void populateMatrix(matrixData *v);
int sumMatrix(matrixData *d, int min, int max);
void classify(float avg);

int threadsAmount = 4;
int *sum;

double time_diff(struct timeval x, struct timeval y);
void main()
{
	int i; //Variables para loop
	//long T1, T2, T3, T4; //Variables para cálculo de tiempo
	float delta = 0, avg;
	srand(time(NULL));
	struct timeval before, after;
	pthread_t threads[threadsAmount];

	matrixData *d = (matrixData *)malloc(sizeof(matrixData)); //Creación datos
	sum = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*sum = 0;

	//Dimensiones matriz
	printf("matrix_rows: ");
	scanf("%d", &d->height);
	printf("matrix_columns: ");
	scanf("%d", &d->width);
	printf("\n");

	populateMatrix(d);
	printf("Height: %d\n", d->height);
	printf("Width: %d\n", d->width);
	printf("\n");
	printMatrix(d);

	/*Thread*/
	//T1 = clock();
	gettimeofday(&before, NULL);
	for (i = 0; i < threadsAmount; i++)
	{
		message *m = (message *)malloc(sizeof(message));
		m->d = d;
		m->part = i; //Asigna a la variable part en cuál hilo estamos (por orden de creación)
		pthread_create(&threads[i], NULL, &functionThread, (void *)m);
	}

	for (i = 0; i < threadsAmount; i++)
	{
		pthread_join(threads[i], NULL); //Espera a que terminen todos los hilos
	}

	avg = (float)*sum / (d->height * d->width);

	//T2 = clock();

	//delta = (float)(T2 - T1) / CLOCKS_PER_SEC;
	//printf("\nTiempo Threads: %.5f segundos\n", delta);
	gettimeofday(&after, NULL);
	printf("Tiempo de ejecución THREADS: %f\n", time_diff(before, after));
	/*Fin Thread*/

	*sum = 0;

	//Inicio Proceso Paralelo

	pid_t worker_1;
	pid_t worker_2;
	pid_t worker_3;
	pid_t worker_4;
	gettimeofday(&before, NULL);
	//  T3 = clock();

	pid_t pid = fork();
	if (pid)
	{
		worker_1 = pid;
		pid = fork();
	}
	if (pid)
	{
		worker_2 = pid;
		pid = fork();
	}
	if (pid)
	{
		worker_3 = pid;
		pid = fork();
		
	}
	if (pid)
		worker_4 = pid;

	if (worker_1 == 0)
	{

		int partialSum = sumMatrix(d, 0, ((d->height * d->width) / 4) - 1);
		*sum += partialSum;
		exit(0);
	}

	if (worker_2 == 0)
	{

		int partialSum = sumMatrix(d, (d->height * d->width) / 4, ((d->height * d->width) / 2) - 1);
		*sum += partialSum;
		exit(0);
	}

	if (worker_3 == 0)
	{

		int partialSum = sumMatrix(d, (d->height * d->width) / 2, (((d->height * d->width) / 4) * 3) - 1);
		*sum += partialSum;
		exit(0);
	}

	if (worker_4 == 0)
	{

		int partialSum = sumMatrix(d, ((d->height * d->width) / 4) * 3, (d->height * d->width) - 1);
		*sum += partialSum;
		exit(0);
	}

	waitpid(worker_1, NULL, 0);
	waitpid(worker_2, NULL, 0);
	waitpid(worker_3, NULL, 0);
	waitpid(worker_4, NULL, 0);

	avg = (float)*sum / (d->height * d->width);

	// T4 = clock();
	//Fin Proceso Paralelo
	gettimeofday(&after, NULL);
	printf("Tiempo de ejecución PROCESOS: %f\n", time_diff(before, after));
	/*delta = (float)(T4-T3)/CLOCKS_PER_SEC;
	printf("\nTiempo Proceso Paralelo: %.5f segundos\n", delta);*/

	printf("\nEl promedio es: %.5f\n", avg);
	classify(avg);

	exit(EXIT_SUCCESS);
}

void printMatrix(matrixData *d)
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
	message *v = (message *)m;
	int part = v->part;
	int partialSum = sumMatrix(v->d, part * (v->d->height * v->d->width) / threadsAmount, ((part + 1) * (v->d->height * v->d->width) / threadsAmount) - 1);
	free(v);
	v = NULL;
	pthread_mutex_lock(&mutex_counter); //Bloquea el hilo para manejar datos (concurrencia)
	*sum += partialSum;					//Agrega la suma parcial al total global
	pthread_mutex_unlock(&mutex_counter);
}

void populateMatrix(matrixData *d)
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

int sumMatrix(matrixData *d, int min, int max)
{
	int sum = 0;
	int i, j;
	//Recorre la matriz como si fuera un array unidimensional, desde un índice mínimo a uno máximo.
	for (i = min; i <= max; i++)
	{
		int row = i / d->width;
		int col = i - (row * d->width);
		sum += d->mat[row][col];
	}
	return sum;
}

void classify(float avg)
{
	if (avg <= 127)
		printf("\nMatriz Oscura\n");
	else
		printf("\nMatriz Clara\n");
}

double time_diff(struct timeval x, struct timeval y)
{
	double x_ms, y_ms, diff;
	x_ms = (double)x.tv_sec * 1000000 + (double)x.tv_usec;
	y_ms = (double)y.tv_sec * 1000000 + (double)y.tv_usec;

	diff = (double)y_ms - (double)x_ms;
	return diff;
}