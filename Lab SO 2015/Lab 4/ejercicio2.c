#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#define MAX_MSJ 100
#define MAX_LINE 1000


char linea[MAX_LINE];

typedef struct{
    int numHilo;
    char mensaje[MAX_MSJ];
    char letraAbuscar[1]; // Se coloco 1, para molestar el molesto WARNING, aun asi no es necesario.
}contadorLetras;


void* fooContLetras(void * p){
   //printf("\n--------------------\n");
    contadorLetras* c=(contadorLetras*)p;
    int idx, cont=0;
    // int numHilo = c->numHilo; char *msj = c->mensaje; char letra = c-> letraAbuscar[1];

    for(idx=0; idx<strlen(c->mensaje); idx++){
        if( c->mensaje[idx]==c->letraAbuscar[0])
            cont++;
    }
    printf("Thread %d: La letra %c aparece %d veces en %s \n",c->numHilo,c->letraAbuscar[0],cont, c->mensaje );
    
    return NULL; // pthread_exit(0) tambien puede usarse.
}

int main(int argc, char *argv[]){
	
    int auxARGC= argc -1;
    contadorLetras arr[auxARGC];
    pthread_t hilos[auxARGC];


    //char linea[] = "Hola mundo !";     // <<<<<<<<<<-----------------------------CAMBIE EL MENSAJE A CONTAR AQUI!!!
    printf("Proporcione el mensaje completo a contar:\n");
    fgets(linea, MAX_LINE, stdin);
    char separador[] = " ";

    char *trozo = NULL;

    trozo = strtok( linea, separador);
    int idx=0;
    while( trozo != NULL && idx<auxARGC) {
        
        arr[idx].numHilo = idx;
        strcpy(arr[idx].mensaje,trozo);
        strcpy(arr[idx].letraAbuscar,argv[idx+1]);

        pthread_create(&hilos[idx],NULL,fooContLetras,(void*)&arr[idx]);
        
        trozo = strtok( NULL, separador);       // Reinicio la variable trozo (Forma 2: Trozo = NUll)
        idx=idx+1;                              // Incremento Idx, para llevar el orden de los threads creados.
    }

    // ciclo para esperar que los hilos terminen su trabajo;
    for (idx=0; idx<auxARGC; idx++){ pthread_join(hilos[idx],NULL);


}
	return 0;
}

