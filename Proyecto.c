#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define maxHeight 1000
#define maxWidth 1000
#define maxValue 255

typedef struct
{
	int height, width;
	int mat[maxHeight][maxWidth];
} datos;

void imprimirMatriz(datos *d);
void llenarMatriz(datos *v);
int sumaMatriz(datos *d, int min, int max);
void clasificar(float avg);

int sum = 0;

int main(){
	
	srand(time(NULL));

	int i, j; //sum = 0;
    long T1;
    long T2;
    float delta, avg = 0;
    
    datos *d = (datos *)malloc(sizeof(datos));

    pid_t worker_1;
    pid_t worker_2;
    pid_t worker_3;
    pid_t worker_4;
    
    printf("matrix_rows: ");
    scanf("%d", &d->height);
    printf("matrix_columns: ");
    scanf("%d", &d->width);
    printf("\n");

    T1 = clock();

    int a[d->height][d->width];

	llenarMatriz(d);
	
	imprimirMatriz(d);

    pid_t pid = fork();
	if (pid) {
		worker_1 = pid;
		pid = fork();
	}
	if (pid) {
		worker_2 = pid;
		pid = fork();
	}
	if (pid) {
		worker_3 = pid;
		pid = fork();
	}
	if (pid)
		worker_4 = pid;

    if(worker_1 == 0) {
        
		int sumaParcial = sumaMatriz(d, 0, ((d->height*d->width)/4)-1);
		sum += sumaParcial;
		printf("\nsumxd1: %d\n", sumaParcial);
        exit(0);
    }

    if(worker_2 == 0) {
        
		int sumaParcial = sumaMatriz(d, (d->height*d->width)/4, ((d->height*d->width)/2)-1);
		sum += sumaParcial;
		printf("\nsumxd2: %d\n", sumaParcial);
        exit(0);
    }

    if(worker_3 == 0) {
        
        int sumaParcial = sumaMatriz(d, (d->height*d->width)/2, (((d->height*d->width)/4)*3)-1);
        sum += sumaParcial;
		printf("\nsumxd3: %d\n", sumaParcial);
        exit(0);
    }

    if(worker_4 == 0) {
        
        int sumaParcial = sumaMatriz(d, ((d->height*d->width)/4)*3, (d->height*d->width)-1);
        sum += sumaParcial;
		printf("\nsumxd4: %d\n", sumaParcial);
        exit(0);
    }
    
    avg = sum/(d->height*d->width);
	printf("\nsumxd: %d\n", sum);
	printf("\navgxd: %f\n", avg);
    
    waitpid(worker_1, NULL, 0);
    waitpid(worker_2, NULL, 0);
    waitpid(worker_3, NULL, 0);
    waitpid(worker_4, NULL, 0);

    T2 = clock();
	delta = (float)(T2-T1)/CLOCKS_PER_SEC;

	printf("\nTime: %.5f segundos\n", delta);
	
	avg = sum/(d->height*d->width);
	printf("\nsum: %d\n", sum);
    printf("\navg: %f\n", avg);

	clasificar(avg);

    return 0;
}

void imprimirMatriz(datos *d) {
	
	int i, j;
	
	for(i = 0; i < d->height; i++) {
		for (j = 0; j < d->width; j++) {
			printf("%d ", d->mat[i][j]);
		}
		printf("\n");
	} 	
}

void llenarMatriz(datos *v) {

	int i, j;
	
	for(i = 0; i < v->height; i++) {
		for(j = 0; j < v->width; j++) {
			v->mat[i][j] = 1+ rand() % maxValue;
		}
	}
}

int sumaMatriz(datos *d, int min, int max)
{
	int i, suma = 0;
	
	for(i = min; i <= max; i++) {
		int row = i / d->width;
		int col = i - (row * d->width);
		suma += d-> mat[row][col];
	}
	return suma;
}

void clasificar(float avg) {
	
	if(avg <= 127)
		printf("\nMatriz Oscura\n");
	else 
		printf("\nMatriz Clara\n");
}
