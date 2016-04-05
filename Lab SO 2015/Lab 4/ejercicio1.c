#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// Realice un código en C usando la biblioteca pthread en el que dada una matriz Anxn se
//obtengan dos vectores: a y b, ambos de tamaño n. El vector a contendrá los resultados de
//las sumas de los elementos de las filas de A y el vector b de la multiplicación de los
//elementos de las columnas. Utilice un hilo para cada par de filas y columnas.
//----
//matrices: las declaramos globales para evitar pasarlas por parametro a los hilos
int **A; // RESULT[1][*]= Suma;  RESULT[2][*]= Multiplicacion;
int n;
int *VECTOR_A;
int *VECTOR_B;
void* suma(void* ptr){
	
	int idx = *((int*)ptr);
	int contSuma=0, contMult=1, jdx;

	for(jdx=0;jdx<n;jdx++){
		contSuma = contSuma + A[idx][jdx];
		contMult = contMult * A[jdx][idx];
	}
	VECTOR_A[idx]=contSuma;
	VECTOR_B[idx]=contMult;

	return NULL; // pthread_exit(0) tambien puede usarse.
}

int main(){
	int idx,jdx, *index;
	printf("Proporcione el n para las matrices:\n");
	scanf("%d",&n);
	// declarando las matrices.
	A=(int**)malloc(n*sizeof(int*));
	for (idx=0; idx<n; idx++) A[idx]=(int*)malloc(n*sizeof(n));
	VECTOR_A=(int*)malloc(n*sizeof(int*));
	VECTOR_B=(int*)malloc(n*sizeof(int*));

	printf("Matriz A?:\n");
	for (idx=0; idx<n; idx++){
		for (jdx=0; jdx<n; ++jdx){
			scanf("%d",&A[idx][jdx]);
		}
	}
	
	
	pthread_t hilos[n];
	for (idx=0; idx<n; idx++){ // ciclo para crear los hilos
		// crear el hilo que trabajara con el cuadrante
			index = (int*)malloc(sizeof(int));
			*index = idx;
		pthread_create(&hilos[idx],NULL,suma,(void*)index);
	}

	// ciclo para esperar que los hilos terminen su trabajo;
	for (idx=0; idx<n; idx++) pthread_join(hilos[idx],NULL);
 	// finalmente mostramos la matriz resultado
	
	printf("\nVector A (Suma):\n");
	for (idx=0; idx<n; idx++){
		//printf("%d - ",RESULT[0][idx]);
		printf("%d ",VECTOR_A[idx]);
	}
	//-----------------
	printf("\nVector B (Multiplicacion):\n");
	for (idx=0; idx<n; idx++){
		//printf("%d - ",RESULT[1][idx]);
		printf("%d ",VECTOR_B[idx]);
	}
	printf("\n   \n");
	return 0;
}
