//Miguel Omaña 17168468
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define QT 3//quantum
#define TAM 10//tamano de la cola
#define MAX_TIME 10//tiempo maximo de ejecucion
#define MIN_PRIORIDAD 10//tiempo minimo de prioridad
#define TEI 3//tiempo de espera para incrementar la prioridad

struct info// estructura de control de procesos
{
	int pid;
	int prioridad;
	int tiempo;
	int t_viejo;
};

struct info cola [TAM];

void llenar_cola()
{
	int i;
	srand(time(NULL));
	for(i=0;i<TAM;i++)
	{
		cola[i].pid = i+1;
		cola[i].prioridad = rand() % MIN_PRIORIDAD + 1 ;
		cola[i].tiempo = rand() % MAX_TIME + 1;
		cola[i].t_viejo = 0;
	}
}

void imprimir_cola()// funcion para imprimir el contenido de la cola, ayudara para depurar el sistema
{
	int i;
	for(i=0;i<TAM;i++)
	{
		printf("Pid es %d \n",cola[i].pid);
		printf("Prioridad es %d \n",cola[i].prioridad);
		printf("Tiempo de ejecucion es %d \n",cola[i].tiempo);
		printf("Tiempo de viejo es %d \n",cola[i].t_viejo);
	}

}

int terminados(){
    int c, i;
    c=0;
    for(i=0;i<TAM;i++){
        if((cola[i].pid) ==-1)
            c++;
    }
    if (c==10)
        return 1;
    else
        return 0;
}

void planificar(){
	int pT=0;
    int i, menor, pE;
	while(pT!=1){
        menor=11; 
        pE=0;
        for(i=0;i<TAM;i++){
            if( (cola[i].prioridad != -1) && ( menor > cola[i].prioridad)){
                menor = cola[i].prioridad;
                pE = i;
			}
        }

        //sleep(QT);
		
        for(i=0;i<TAM;i++){
            if(i==pE){ 
                cola[pE].prioridad = menor + 1 ;
                cola[pE].tiempo = (cola[pE].tiempo-QT);
                cola[pE].t_viejo = 0;
            } else       
                if(cola[i].pid != -1) {
                    cola[i].t_viejo = (cola[i].t_viejo + 1);
                    if(cola[i].t_viejo > TEI){
                        cola[i].prioridad=(cola[i].prioridad-1);
                        cola[i].t_viejo=0;
                    }
                }
        }
        if (cola[pE].tiempo <= 0){ 
            cola[pE].pid =-1;
            cola[pE].prioridad = -1;
            cola[pE].tiempo = -1;
            cola[pE].t_viejo = -1;
        }
		pT=terminados(); 
    }
}
int main(){

    llenar_cola();
    imprimir_cola();
    planificar();
    imprimir_cola();/* Luego de planificar() el contenido de todos los campos de los procesos en la cola deberian ser -1*/

	return 0;
}
