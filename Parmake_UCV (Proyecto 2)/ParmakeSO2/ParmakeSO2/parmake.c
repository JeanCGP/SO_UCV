/* ********************************************
// ***   Universidad Central de Venezuela 	***
// ***        Facultad de Ciencias		 	***
// *** Sistema Operativo (6004) -Seccion C1 ***
// *** Proyecto #2 - Make Paralelo(Parmake) ***
// *** Elaborado por: 						***
// ***   Jean Carlos Gomes & Miguel Omaña   ***
// *** fecha: 16/07/2015                    ***
// ********************************************
*/

// ***  LIBRERIAS ***
#include <pthread.h>
#include <semaphore.h>
#include <getopt.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parmake.h"
#include "queue.h"
#include "parser.h"


// *** Variables Globales ***

queue_t *tbd_queue = NULL;  				//to be done queue.
pthread_t *threads;  						//Working Threads.
pthread_attr_t stack; 						//Attribute Threads.
targs_t *args; 								// Argument Threads.
sem_t *sems, queue_sem, bool_sem, mutex; 	//Some semaphores.
int32_t num_threads = 0, ind = 0, numExe=0; // Auxiliar Variables.
bool double_keys = FALSE, *key_locked; 		//Support Arrays for Threads.


// *** FIRMAS ***

void clean_up(void);
void error_opt(opt_errors_t error, char, char *);
void parsed_new_key(char *);
void parsed_command(char *, char *);
void parsed_dependency(char *, char *);
void init_make(make_t *);
bool search_key(char *, queue_t*);
void ciclosInexistente();
void *thread_func();
void key_request(order_t *);
void key_notify(char *);

// *** MENU PRINCIPAL ***

int main(int argc, char **argv){
  										// Declaracion de variables locales.
    									// Enteros auxiliares para semaforos.
    									// Indices para ciclos.
    									// Apuntadores a makes auxiliares.
    									// Booleanos para comprobacion de errores.
    									// Cadenas auxiliares.
 	int i, num_threads=0,entrada;
  	char *directorio=malloc(256), *path=malloc(256);
  
  	atexit(clean_up);
  
    									// Examinar y validar los parametros del programa.
    									// Solo se aceptan los parametros -"j #" y "<nombre-archivo>".
    									// Verificacion de errores.
    
  	while ((entrada = getopt(argc, argv,"j:")) != -1) {
        switch (entrada) {
            case 'j' : num_threads = atoi(optarg);
                break;
        }
    }
 	getcwd(directorio,256);
  	strcpy(path,directorio);
  	strcat(path,"/"); strcat(path,argv[3]);

  	if (access(path, F_OK) != 0){		// Determinar si el archivo especificado existe.
		printf("El makefile especificado no existe\n"); exit(1);
  	}  
  
  	
  	tbd_queue = alloc_queue();			// Preparar la cola y analizar el archivo.
  	parser_parse_makefile(argv[3],&parsed_command,&parsed_new_key,&parsed_dependency);
  
  										// buscar dependencias ciclicas y dependencias inexistentes.
      									// Primero las dependencias inexistentes (las faciles).
      									// Terminar la ejecucion si se encontraron dependencias inexistentes o claves repetidas.
      									// Luego verificar por dependencias ciclicas.
      									// Terminar la ejecucion si se encontraron dependencias ciclicas o inexistentes.
  	ciclosInexistente();
  
      									// Inicializar las viarables.
	threads=(pthread_t*)malloc(num_threads*sizeof(pthread_t)); 
	sems=(sem_t*)malloc(num_threads*sizeof(sem_t));
	for(i=0;i<num_threads;i++){
		sem_init(&sems[i], 0, 0); 		
	}
	sem_init(&queue_sem,0,1); sem_init(&bool_sem,0,1); sem_init(&mutex,0,1);
	
  										// Lanzar los hilos trabajadores.
	for(i=0; i<num_threads;i++){									// <<------- TRUCO CASTEO (intptr_t)
		if (pthread_create(&threads[i], NULL, (void *)&thread_func,(void*)(intptr_t)i) != 0) {
			printf("El Thread  numero: %d no pudo ser creado con exito\n", i); 
		}
	}
	
	for (i=0; i<num_threads; i++){ 		// Esperar por los hilos trabajadores.	
		if(pthread_join(threads[i],NULL) != 0) { 
			printf("El Thread numero: %d no pudo ser esperado\n",i);	
		}
	}
	
	clean_up();
	
  	return 0;
}

void clean_up(void){
  	// Limpiar toda la memoria al finalizar la ejecucion.
	int i;
	
	//Se destruye los semaforos utilizados
    sem_destroy (&queue_sem); sem_destroy (&bool_sem); sem_destroy (&mutex);
	for(i=0;i<numExe;i++){
		sem_destroy(&sems[i]);
	}
	
	//Liberamos la memoria de la cola
	make_t *make;
    while(queue_size(tbd_queue) > 0) {
       	make = (make_t *) queue_pop_front(tbd_queue);
       	free(make);
    }    
}

void error_opt(opt_errors_t error, char opt, char *arg){
  // Notificar los errores posibles al analizar los parametros del programa.
 	switch(error){
  		case OPT_WRNG_PARAM:
    	printf("The option %c does not recognize %s as a parameter.\n", opt, arg);
    	exit(1);
    	break;
  	case OPT_INV_OPT:
    	printf("The option %c is not a valid option.\n", opt);
    	exit(1);
    break;
  	default:
   	 	printf("void error_opt() has been called with wrong parameters.\n");
    	exit(1);
    break;
  	}
}

void parsed_new_key(char *key){
	// Procesar la clave leida por el parser y agregarla a la cola.
	char* keyDup=malloc(strlen(key));
	int i;

	strcpy(keyDup,key);					// Duplicar la cadena "key".
	
										//Verificar que no exista doble definición
	for(i=0;i<(int)queue_size(tbd_queue);i++){
		make_t* makeCmp; makeCmp=(make_t*)malloc(sizeof(make_t));
		makeCmp=queue_at(tbd_queue,i);
		if(strcmp(key,makeCmp->key)==0){
			printf("La Clave:%s esta duplicada\n",makeCmp->key);
			exit(1);
		}
		makeCmp=NULL; free(makeCmp);
	}
	
									// Inicializar una nueva estructura "make".
	make_t *make;
	make=(make_t*)malloc(sizeof(make_t));
	init_make(make);
	strcpy(make->key,keyDup);	
							
	queue_push_back(tbd_queue,(void*)make);	// Encolar la estructura creada.
								
	keyDup=NULL;free(keyDup);			// Liberar memoria y terminar.
	make=NULL; free(make);

  return;
}

void parsed_command(char *key, char *command){
  // Agregar el comando leido por el parser a la cola de su clave correspondiente.
	int i;
	char* keyDup=malloc(strlen(key));
	char* comDup=malloc(strlen(command)+1);
	
  // Copiar las cadenas de entrada.
  	strcpy(keyDup,key); strcpy(comDup,command);

  // Agregar el comando a la lista de la clave actual.
	for(i=0;i<(int)queue_size(tbd_queue);i++){
		make_t *makeAux;	makeAux=(make_t*)malloc(sizeof(make_t));
		makeAux=(make_t*)queue_at(tbd_queue,i);
		if(strcmp(keyDup,makeAux->key)==0){
			queue_push_back(makeAux->comms,comDup);
			makeAux->ncom++;
			i=queue_size(tbd_queue);	
		}
		makeAux=NULL; free(makeAux);
	}

   	keyDup=NULL; free(keyDup);		// Liberar memoria y salir.
  	comDup=NULL; free(comDup);		// Liberar memoria y salir.

  	return;
}

void parsed_dependency(char *key, char *dependency){
  // Agregar la dependencia leida por el parser a la cola de su clave correspondiente.
	int i;
	char* keyDup=malloc(strlen(key));
	char* depDup=malloc(strlen(dependency));

  // Copiar las cadenas de entrada.
	strcpy(keyDup,key); strcpy(depDup,dependency);
	
	// Agregar la dependencia a la lista de la clave actual.
	for(i=0;i<(int)queue_size(tbd_queue);i++){
		make_t *makeAux;	makeAux=(make_t*)malloc(sizeof(make_t));
		makeAux=(make_t*)queue_at(tbd_queue,i);
		if(strcmp(keyDup,makeAux->key)==0){
			// Verificar si esta dependencia es repetida.
			if(search_key(depDup,makeAux->deps)==FALSE){
				// Si la dependencia no es repetida se agrega a la cola.
				queue_push_back(makeAux->deps,depDup);
				makeAux->ndep++;
				i=queue_size(tbd_queue);
			}
		}
		makeAux=NULL; free(makeAux);	
	}

  // Liberar memoria y salir.
  keyDup=NULL; free(keyDup);
  depDup=NULL; free(depDup);
	
}

void init_make(make_t *make){
  // Inicializar una estructura Make.
		make->key=malloc(256);
		make->deps=NULL; make->deps=alloc_queue();
		make->comms=NULL; make->comms=alloc_queue();
		make->ndep=0; make->ncom=0; make->ind=ind; ind++;
		make->visit=FALSE; make->exec=FALSE;
  }

bool search_key(char *key, queue_t* q){
  // Verificar si la clave key se encuentra en la cola q.
  int tamCola, i;
  tamCola=queue_size(q);
  
  if(tamCola==0){
		 return FALSE;
	}else{
			for(i=0;i<tamCola;i++){
				if(strcmp(key,queue_at(q,i))==0){
							return TRUE;
				}
			}
		}		
  return FALSE;
}

bool test_cycle(const make_t *o_key, make_t *t_key){
  							// Busqueda de ciclos en grafos usando BFS recursivo.
	    					// Caso base: Si la clave de prueba t_key no tiene dependencias,
    						// entonces no puede formar ciclos.
  	if(t_key->ndep == 0){
    	t_key->visit = TRUE;
    	return FALSE;
  	}else if(!strcmp(t_key->key, o_key->key)){
    						// Si la clave de prueba t_key es igual a la clave de origen o_key,
    						// Entonces hay un ciclo.
    	t_key->visit = TRUE;
    	return TRUE;
  	}else{
    						// Paso recursivo.
    	t_key->visit = TRUE;
    	int32_t i, j;
    	make_t *m;
    	char *dep;
    	for(i = 0; i < queue_size(t_key->deps); ++i){
      		for(j = 0; j < queue_size(tbd_queue); ++j){
        		m = (make_t*)queue_at(tbd_queue, j);
        		dep = (char*)queue_pop_front(t_key->deps);
        		assert(dep != NULL);
        		queue_push_back(t_key->deps, (void*)dep);
        		if(!strcmp(m->key, dep))
        			break;
        		else
          			m = NULL;
      		}
      	if(m != NULL && !m->visit)
			if(test_cycle(o_key, (make_t*)queue_at(tbd_queue, j)))
          		return TRUE;
    	}
    return FALSE;
  	}
}

void ciclosInexistente(){
	//Para validar que no exista una dependencia a una clave inexistente
	
	int i,j,k;
	
	for(i=0;i<(int)queue_size(tbd_queue);i++){
		make_t *makeAux; makeAux=(make_t*)malloc(sizeof(make_t));
		makeAux=queue_at(tbd_queue,i);
		if(makeAux->ndep > 0){
			for(j=0;j<(int)queue_size(makeAux->deps);j++){
				char* cadAux; cadAux=(char*)malloc(256*sizeof(char));
				cadAux=queue_at(makeAux->deps,j);
				bool enc=FALSE;
				for(k=0;k<(int)queue_size(tbd_queue);k++){
					make_t *makeCmp; makeCmp=(make_t*)malloc(sizeof(make_t));
					makeCmp=queue_at(tbd_queue,k);
					if(strcmp(cadAux,makeCmp->key)==0){
						enc=TRUE;
					}
					makeCmp=NULL; free(makeCmp);
				}
				if(enc==FALSE){
					printf("La Dependencia: %s es inexistente\n",cadAux);
					exit(1);
				}
				cadAux=NULL; free(cadAux);
			}
		}
		makeAux=NULL; free(makeAux);
	}
}

void *thread_func(){
  // Funcion de ejecucion de los hilos trabajadores.
  
  // Declaracion de variables locales.
  int i;
  bool finish=FALSE;
  order_t *orden=(order_t*)malloc(sizeof(order_t));
  orden->key=(make_t*)malloc(sizeof(make_t));
	
	// Entrada al ciclo de Solicitud-Ejecucion
	while(finish==FALSE){
		// Se solicita una orden al metodo protector de la cola.
		key_request(orden);
	
		switch(orden->type){
		// Si la orden es terminar la ejecucion, marcamos finish como TRUE
		// para detener el ciclo en la proxima iteracion.
		case O_TERMINATE:
			sem_wait(&mutex);
			finish=TRUE;
			sem_post(&mutex);
		break;
		// Si la orden es ejecutar, bloqueamos su semaforo, y ejecutamos todos sus comandos.
		case O_EXECUTE:
			for(i=0;i<orden->key->ncom;i++){
				system(queue_at(orden->key->comms,i));
			}
			//Luego marcamos la clave como ejecutada
			sem_wait(&queue_sem); orden->key->exec=TRUE; numExe++;sem_post(&queue_sem);
			//Llamamos al metodo para notificar que las claves que dependan de esta deben ser acomodadas.
			key_notify(orden->key->key);
	
		break;
		// Si la orden es esperar, bloqueamos su semaforo, y esperamos que se ejecuten todas las claves de las que depende.
		case O_WAITFOR:
			sem_wait(&sems[orden->key_ind]);
			//Una vez despertado el hilo,  ejecutamos todos sus comandos.
			//printf("LA CLAVE EJECUTADA ES:%s y su hilo es:%d\n",orden->key->key,id);
			for(i=0;i<orden->key->ncom;i++){
				system(queue_at(orden->key->comms,i));
			}
			//Luego marcamos la clave como ejecutada
			sem_wait(&queue_sem); orden->key->exec=TRUE; numExe++;sem_post(&queue_sem);
			//Llamamos al metodo para notificar que las claves que dependan de esta deben ser acomodadas.
			key_notify(orden->key->key);
		break;
		// Como default lanzamos un error porque nunca deberia entrarse en este caso.
		default:
			printf("Error en orden recibida\n"); exit(1);
		break;	
		}		
	} 
  pthread_exit(NULL);
}

void key_request(order_t *order){
  // Funcion protectora de la cola. Sirve para examinar la cola para buscar
  // claves que ejecutar respetando la exclusion mutua.
  
  // Declaracion de variables locales.
 int i;
 bool enc=FALSE;
 
  // Bloqueamos el acceso a la cola.
  sem_wait(&queue_sem);
  
  // Si la cola esta vacia, indicamos que hay que terminar y liberamos la cola.
  // O lo que es lo mismo si ya se ejecutaron todas las claves  
  if(numExe>=(int)queue_size(tbd_queue)){
	order->type=O_TERMINATE;
	enc=TRUE;
  }else{	// Sino, examinamos toda la cola.
			for(i=0;i<(int)queue_size(tbd_queue);i++){
			  make_t *makeAux;	makeAux=(make_t*)malloc(sizeof(make_t));
			  makeAux=(make_t*)queue_at(tbd_queue,i);
				// Cuando encontremos un elemento que no este asignado, que no haya sido
				// ejecutado, y que no tenga dependencias, indicamos que hay que 
				// ejecutar ese elemento y lo marcamos como asignado.
				if(makeAux->ndep==0 && makeAux->visit==FALSE && makeAux->exec==FALSE){
					makeAux->visit=TRUE;
					order->type=O_EXECUTE;
					order->key_ind=makeAux->ind;
					order->key=makeAux;
					i=queue_size(tbd_queue); enc=TRUE;	
				}
			  makeAux=NULL; free(makeAux);
			}
		}
  // Liberamos el acceso a la cola.
  // Si encontramos un elemento a ejecutar en el paso anterior lo retornamos.
  sem_post(&queue_sem);
  
  // Sino, volvemos a bloquear el acceso a la cola.
  if(enc==FALSE){
	sem_wait(&queue_sem);
		// Volvemos a recorrer toda la cola.
		for(i=0;i<(int)queue_size(tbd_queue);i++){
			make_t *makeAux;	makeAux=(make_t*)malloc(sizeof(make_t));
			makeAux=(make_t*)queue_at(tbd_queue,i);
			// Cuando encontremos un elemento que tenga dependencias pero que no
			// haya sido ejecutado ni asignado, indicamos que hay que esperar por el.
			if(makeAux->ndep>0 && makeAux->visit==FALSE && makeAux->exec==FALSE){
					makeAux->visit=TRUE;
					order->type=O_WAITFOR;
					order->key_ind=makeAux->ind;
					order->key=makeAux;
					i=queue_size(tbd_queue); enc=TRUE;
			}
			makeAux=NULL; free(makeAux);
		}
	// Liberamos el acceso a la cola.	
	sem_post(&queue_sem);
  }
  // Si no encontramos ningun elemento en el paso anterior, indicamos
  // que hay que terminar.
  if(enc==FALSE){
	  sem_wait(&mutex);
	  order->type=O_TERMINATE;
	  sem_post(&mutex);
  } 
}

void key_notify(char *key){
  // Funcion protectora de la cola para actualizar las claves a medida de
  // son ejecutadas.
  
  // Declaracion de variables locales.
	int i,j;
  // Bloqueamos el acceso a la cola.
	sem_wait(&queue_sem);
	
	// Aqui Si la dependencia es igual a la clave que fue ejecutada, la eliminamos
	
	// Revisamos toda la cola, para evaluar cada key y sus dependencias.
	for(i=0;i<(int)queue_size(tbd_queue);i++){
		
		make_t *makeAux;	makeAux=(make_t*)malloc(sizeof(make_t));
		makeAux=(make_t*)queue_at(tbd_queue,i);
		//Cola auxiliar donde estaran todas las dependencias menos la que ya fue ejecutada
		queue_t *aux_queue = NULL; aux_queue=alloc_queue(); 
	
		 // Revisamos cada dependencia del elemento que estamos examinando.
		for(j=0;j<(int)queue_size(makeAux->deps);j++){
			//Encolamos todas las dependencias menos la que fue ejecutada
			if(strcmp(key,queue_at(makeAux->deps,j))!=0){			
				char* depCmp=(char*)malloc(256*sizeof(char));
				depCmp = (char *)queue_at(makeAux->deps,j);
				queue_push_back(aux_queue,(char*)depCmp);
				depCmp=NULL; free(depCmp);
			}
		}
		
		//Desencolamos todas las dependencias del elemento que estamos analizando
		char *dep;
		while(queue_size(makeAux->deps) > 0) {
			dep = (char *) queue_pop_front(makeAux->deps);
			dep=NULL; free(dep);
			 makeAux->ndep--; 
		}
		
		//La cola de la clave que estamos evaluando la igualamos a la cola auxiliar que no tiene
		//a la clave que fue ejecutada
		for(j=0;j<(int)queue_size(aux_queue);j++){
			char* depEnc; depEnc=(char*)malloc(256*sizeof(char));
			depEnc=(char*)queue_at(aux_queue,j);
			queue_push_back(makeAux->deps,(char*)depEnc); makeAux->ndep++;
			depEnc=NULL; free(depEnc);
		}
		
		//Si ademas esta era la ultima dependencia, despertamos
        // al hilo que este esperando por ella.
		if(makeAux->ndep==0) sem_post(&sems[makeAux->ind]);
	}

	// Liberamos el acceso a la cola.
	sem_post(&queue_sem);
 
}
