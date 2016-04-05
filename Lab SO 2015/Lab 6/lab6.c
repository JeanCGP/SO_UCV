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

void planificar()
{
	/*
		CODIGO PLANIFICAR
	*/
		int procesos_listos=0, i;
	while(procesos_listos<30)	
	{

		int pos_proceso;
		struct info aux;
		pos_proceso=0;
		aux.prioridad=99999999;
		for(i=0;i<TAM;i++)
		{
			if (aux.prioridad > cola[i].prioridad ) // se busca el proceso con menor prioridad
			{ 										//  2. 	La prioridad será en orden descendente, es decir, un proceso con prioridad 1 (uno)
			aux.prioridad=cola[i].prioridad;		//    	tendrá mayor prioridad que un proceso con prioridad 5 (cinco).
			aux.pid=cola[i].pid;
			aux.tiempo=cola[i].tiempo;
			aux.t_viejo=cola[i].t_viejo;
			pos_proceso=i;
			}
		}
		printf("La Prioridad del proceso eleguido es %d \n",aux.prioridad); // 3. Se deberá elegir al proceso con mayor prioridad de toda la cola para su ejecución.
		if(aux.prioridad>10)  procesos_listos=30;
		//sleep(QT);															// 4. El proceso elegido se ejecutara solo por un quantum de tiempo
		// justo luego de q pase el quamtum antes de entrar a la cola nuevamente
		if(cola[pos_proceso].tiempo < 0) procesos_listos=procesos_listos+1; // Terminar Procesos.
		aux.tiempo=aux.tiempo - QT; 				// 5. El proceso volverá a la cola con su tiempo restante luego de ejecutar su quantum.
		// PARTE 6: No se desea favorecer eternamente a los procesos con mayor prioridad...
		aux.prioridad=aux.prioridad + 1; 			// Requerimiento 6.A
		aux.t_viejo=aux.t_viejo	+ 1;				// <<---------------------------------------
		aux.t_viejo=0; 								// Requerimiento 6.C
		
		//regresando el proceso a la cola 				Requerimiento 6.D
		cola[pos_proceso].pid =aux.pid;
		cola[pos_proceso].prioridad =aux.prioridad ;
		cola[pos_proceso].tiempo =aux.tiempo;
		cola[pos_proceso].t_viejo =aux.t_viejo;
		printf("El proceso que regreso a la cola es el %d \n", cola[pos_proceso].pid);
		
	// revisando si hay procesos q terminaron y aumentos el contador para terminar, condicion de parada	
		for(i=0;i<TAM;i++)
		{										// Requerimiento 6.B
			if (cola[i].tiempo==-1 ) // se busca el proceso con menor prioridad
			{ 
				procesos_listos=procesos_listos + 1;
				cola[i].tiempo =-2;
				//cola[pos_proceso].pid =-2;
				
			}
		}
		
	}
	
	for(i=0;i<TAM;i++)		// 7. Cuando un proceso culmina debe establecer los campos de su estructura de contro en -1
		cola[i].tiempo = -1;
}
int main()
{
	llenar_cola();
	imprimir_cola();
	planificar();
	imprimir_cola();/* Luego de planificar() el contenido de todos los campos de los procesos en la cola deberian ser -1*/
	return 0;
}
