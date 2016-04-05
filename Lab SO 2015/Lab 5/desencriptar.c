#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define  ENE "-3"  //<--------------  CAMBIE EL VALOR DE N, ESTE N, sera la CLAVE!!!!

int hilos_entradaActivo;
int active_work;
void *KEY;
int bufSize = 0;
FILE *file_in;
FILE *file_out;


pthread_mutex_t mutexIN;
pthread_mutex_t mutexWORK;
pthread_mutex_t mutexOUT;


typedef struct{
	char data;
	off_t offset;
	char state;
} BufferItem;

BufferItem *result;


void thread_sleep(void);
int estaELbufferVacio();
int vaciarArchivoAbuffer();
int hilosTrabajadoresSobreBuffer();
int hilosEncriptadoresSobreBuffer();
void inicializarBuffer();
void *IN_thread(void *param);
void *WORK_thread(void *param);
void *OUT_thread(void *param);




int main(int argc, char *argv[]){
	int i = 0;
	int nIN;
	int nOUT;
	int nWORK; 
	

	pthread_mutex_init(&mutexIN, NULL);
	pthread_mutex_init(&mutexWORK, NULL);
	pthread_mutex_init(&mutexOUT, NULL);


	
	
	if(strcmp(argv[1],"-e")==0) nIN = atoi(argv[2]); 
	nWORK=nIN; 
	KEY = ENE;
	if(strcmp(argv[3],"-d")==0) nOUT = atoi(argv[4]); 
	if(strcmp(argv[5],"-m")==0) file_in = fopen(argv[6], "r");
	if(strcmp(argv[7],"-f")==0) file_out = fopen(argv[8], "w");
	bufSize = 5;
	hilos_entradaActivo = nIN;
	active_work = nWORK;

	// threads
	pthread_t INthreads[nIN];
	pthread_t OUTthreads[nOUT];
	pthread_t WORKthreads[nWORK];
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	// create a buffer item and initialize the buffer
	result = (BufferItem*)malloc(sizeof(BufferItem)*bufSize);
	inicializarBuffer();

	// error handling for user input
	int keyCheck = atoi(KEY);


	for (i = 0; i < nIN; i++){
		pthread_create(&INthreads[i], &attr, (void *) IN_thread, file_in);
	}
	for (i = 0; i < nWORK; i++){
		pthread_create(&WORKthreads[i], &attr, (void *) WORK_thread, KEY);
	}
	for (i = 0; i < nOUT; i++){
		pthread_create(&OUTthreads[i], &attr, (void *) OUT_thread, file_out);
	}

	
	for (i = 0; i < nIN; i++){
		pthread_join(INthreads[i], NULL);
	}
	for (i = 0; i < nWORK; i++){
		pthread_join(WORKthreads[i], NULL);
	}
	for (i = 0; i < nOUT; i++){
		pthread_join(OUTthreads[i], NULL);
	}

	
	pthread_mutex_destroy(&mutexIN);
	pthread_mutex_destroy(&mutexOUT);
	pthread_mutex_destroy(&mutexWORK);

	
	fclose(file_in);
	fclose(file_out);
	free(result);

	return 0;
}



void thread_sleep(void){
	struct timespec t;
	int seed = 0;
	t.tv_sec = 0;
	t.tv_nsec = rand_r((unsigned int*)&seed)%(10000000+1);
	nanosleep(&t, NULL);
}

int estaELbufferVacio(){
	int i = 0;
	while (i < bufSize){
		if (result[i].state == 'e'){
			return 1;
		}
		i++;
	}
	return 0;
}


int vaciarArchivoAbuffer(){
	int i = 0;
	if (estaELbufferVacio()){
		while (i < bufSize){
			if (result[i].state == 'e'){
				return i;
			}
			i++;
		}
	}
	return -1;
}

int hilosTrabajadoresSobreBuffer(){
	int i = 0;
	while (i < bufSize){
		if (result[i].state == 'w'){
			return i;
		}
		i++;
	}
	return -1;
}

int hilosEncriptadoresSobreBuffer(){
	int i = 0;
	while (i < bufSize){
		if (result[i].state == 'o'){
			return i;
		}
		i++;
	}
	return -1;
}

void inicializarBuffer(){
	int i = 0;
	while (i < bufSize){
		result[i].state = 'e';
		i++;
	}
}




void *IN_thread(void *param){
	int index;
	char curr;
	off_t offset;

	thread_sleep();
	
	do {
		
		pthread_mutex_lock(&mutexWORK);
		index = vaciarArchivoAbuffer();			

		while (index > -1){

	
			if (estaELbufferVacio() == 1) {
				thread_sleep();
			}
			
	
			pthread_mutex_lock(&mutexIN);
			offset = ftell(file_in);
			curr = fgetc(file_in);
			pthread_mutex_unlock(&mutexIN);

			if (curr == EOF){
				break;
			}

			else{ 
				
				result[index].offset = offset;
				result[index].data = curr;
				result[index].state = 'w';
				index = vaciarArchivoAbuffer();
			}
		}

		pthread_mutex_unlock(&mutexWORK);

	} while (!feof(file_in));

	thread_sleep();	

	
	pthread_mutex_lock(&mutexWORK);
	hilos_entradaActivo--;
	pthread_mutex_unlock(&mutexWORK);
	
	return NULL;
}

void *WORK_thread(void *param){
	int index = 0;
	int local_active_in;
	char curr;
	int key = atoi(param);
	
	thread_sleep();

	do {
		
		pthread_mutex_lock(&mutexWORK);
		index = hilosTrabajadoresSobreBuffer();
		
		if (index > -1){
			
			
			curr = result[index].data;
			
			
			if (estaELbufferVacio() == 1) {
				thread_sleep();
			}
			
			if (curr == EOF || curr == '\0'){
				break;
			}
			
			
			if (key >= 0 && curr > 31 && curr < 127){
				curr = (((int)curr-32)+2*95+key)%95+32;
			}
			else if (key < 0 && curr > 31 && curr < 127){
				curr = (((int)curr-32)+2*95-(-1*key))%95+32;
			}

			
			result[index].data = curr;
			result[index].state = 'o';
		}
		
		local_active_in = hilos_entradaActivo;
		pthread_mutex_unlock(&mutexWORK);

	} while (index > -1 || local_active_in > 0);

	thread_sleep();	

	pthread_mutex_lock(&mutexWORK);
	active_work--;
	pthread_mutex_unlock(&mutexWORK);

	return NULL;
}


void *OUT_thread(void *param){
	int index = 0;
	char curr;
	off_t offset;
	int local_active_work;

	thread_sleep();

	do {

		pthread_mutex_lock(&mutexWORK);
		index = hilosEncriptadoresSobreBuffer();

		if (index > -1){

			
			offset = result[index].offset;
			curr = result[index].data;

			
			if (estaELbufferVacio() == 1) {
				thread_sleep();
			}

			pthread_mutex_lock(&mutexOUT);
			if (fseek(file_out, offset, SEEK_SET) == -1) {
    			fprintf(stderr, "error %u\n", (unsigned int) offset);
    			exit(-1);
			}
			if (fputc(curr, file_out) == EOF) {
    			fprintf(stderr, "error %d \n", curr);
   				 exit(-1);
			}
			pthread_mutex_unlock(&mutexOUT);

			result[index].data = '\0';
			result[index].state = 'e';
			result[index].offset = 0;
		}

		local_active_work = active_work;
		pthread_mutex_unlock(&mutexWORK);
	
	} while (index > -1 || local_active_work > 0);
	
	thread_sleep();
	
	return NULL;
}
