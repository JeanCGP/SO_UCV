// +-+-+- LIBRERI	AS -+-+-+
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>  
#include <string.h>  // AnalizarOrden
#include <unistd.h>   // Fork();	
#include <signal.h>  // Historial (Crtl^C)
#include <errno.h>  

#include <sys/resource.h> // time
#include <sys/time.h>     // time

// +-+-+- CONSTANTES -+-+-+
#define TAMANO_ORDEN  512
#define MAX_ARGS    64
#define MAX_ARG_LEN 16
#define MAX_PATHS   64
#define LONG_MAX_PATH    512
#define ESPACIOBLANCO  " ,\t"

//Historial-Miguel
#define MAX_CMD_LEN  128
#define HISTORY_COUNT 10

#ifndef NULL
#define NULL    0
#endif


 // +-+-+- V.GLOBALES -+-+-+
int flag = 1,flag2=1;
char *hist[HISTORY_COUNT]; //Miguel
int current = 0;
 
char historial[10][TAMANO_ORDEN];
int indiceHist=0;
char *hist[HISTORY_COUNT];

struct comando_t 
{
  char *nombre;
  int argc;
  char *argv[MAX_ARGS];
};

/*
struct historial_struct
{
  char historial[10][TAMANO_ORDEN];
  int indice;
  int total;
};
*/

// +-+-+- FIRMAS -+-+-+
typedef void (*sighandler_t)(int);
void mostrarIndicador();
void leerOrden(char *);
char *buscarPath(char **, char **);
int analizarOrden(char *, struct comando_t *);
int analizarPath(char **);
void catch(int);
int clear_history();
extern FILE *stdin;
void uswtime(double *, double *, double *);



// +-+-+- MENU PRINCIPAL -+-+-+
int main() 
{
    char comandoLine[TAMANO_ORDEN];
    char *pathv[MAX_PATHS];
    char * buf = 0;
    int numPaths, i, j, len, flag3=0;
    int chPID, estado;	// FORK()	
    pid_t thisChPID;    // FORK()
    struct comando_t comando; //AnalizarOrden
	
	//Miguel
    int h = 0;

  // Jean 
    double utime0, stime0, wtime0, utime1, stime1, wtime1;
    int flagTime =0;

  //-----
	
	signal(SIGINT, SIG_IGN);
	signal(SIGINT, catch);
	
	for(i=0; i<MAX_ARGS; i++)
        comando.argv[i] = (char *) malloc(MAX_ARG_LEN);

    analizarPath(pathv);

    /*char crtl=*///signal(SIGINT, &catch);
	

	 for (h = 0; h < HISTORY_COUNT; h++)
        hist[h] = NULL;
	

	while(flag)                           // Ciclo infinito mientras sea verdadero...
	{
	
			  buf = getcwd(buf,2048);
			  flag3=0;
			  if(flag2==1)
				mostrarIndicador();               // Muestro el indicador por el terminal, una vez hecho CD
			  else                                // Se pierde esto y moestrara el indicador + la ruta
				printf("JeanC-Miguel@JeanC-Miguel-Desktop/%s$> ",buf); 
       
				leerOrden(comandoLine);             // Leo el Comando y le quito el ultimo caracter leido por fgets (\0)
        uswtime(&utime0, &stime0, &wtime0);

        

			  if( (strcmp(comandoLine, "exit") == 0) || (strcmp(comandoLine, "quit") == 0)) break;	// Salgo del Shell
			  if(strncmp(comandoLine,"cd",2)==0 ){  // cd tiene que ser ejecutado por model del (CHDIR)
													// como funcion me arrojaba error en el pase de parametro
			   if(comandoLine[2]!=' '){             // por lo tanto llame chdir en el main...
				  flag2=0; flag3=1;                 // imprimo la ruta, concatenando lo leido sacando el substring cd
				  int ret;
				  printf("\nRuta = %s\n",(comandoLine+2));
				  ret = chdir ((comandoLine+2));
				  if(ret!=0){                       // Si ret!=0 Significa que no se encontro la rura 
					perror("Error 001:");           // para usar lo visto en el lab, uso perror para impirmir un codigo de error
				  }else{                            // Si la ruta es correcta concateno una barra, esto me permite que mi funcion
					strcat(buf,"/");                // cd lea carpetas con espacios...por lo tanto tengo dos if...
					strcat(buf,(comandoLine+2));
				  }
				}
				if(comandoLine[2]==' '){            // Este Segundo if, hace lo ya explicado... 
				  flag2=0; flag3=1;                 
				  int ret;
				  printf("\nRuta = %s\n",(comandoLine+3));
				  ret = chdir ((comandoLine+3));
				  if(ret!=0){
					perror("Error 001:");
				  }else{
					strcat(buf,"/");
					strcat(buf,(comandoLine+3));
				  }
				}

			  }

        //if(strncmp(comandoLine,"time",4)==0 ){
			         //uswtime(&utime0, &stime0, &wtime0);
               //flagTime=1;

       // }
			  	/* ****************************************
			   **    Historal de Comandos              **
			   ******************************************
			   * El historal de comando, lo aplique mediante un arreglo global de caracteres
			   * este solo debe llevar un indice y ejecutar el comando que esta en el arreglo
			   * como son 10 comando lo hice estatico, aplicando el indice 'mod' 10...
			  */
			  
			    //fgets(comandoLine, TAMANO_ORDEN, stdin);
                //if (comandoLine[strlen(comandoLine) - 1] == '\n')
                    //    comandoLine[strlen(comandoLine) - 1] = '\0';

                free(hist[current]);
                hist[current] = strdup(comandoLine);
                current = (current + 1) % HISTORY_COUNT;

            


                if( (strcmp(comandoLine, "r") == 0)){         
                    printf("%s\n", hist[current-2] );
                    strcpy(comandoLine,hist[current-2]);
                }
                if( (strcmp(comandoLine, "r 0") == 0)){           
                    printf("%s\n", hist[0] );
                    strcpy(comandoLine,hist[0]);
                }
                if( (strcmp(comandoLine, "r 1") == 0)){           
                    printf("%s\n", hist[1] );
                    strcpy(comandoLine,hist[1]);
                }
                if( (strcmp(comandoLine, "r 2") == 0)){           
                    printf("%s\n", hist[2] );
                    strcpy(comandoLine,hist[2]);
                }
                if( (strcmp(comandoLine, "r 3") == 0)){           
                    printf("%s\n", hist[3] );
                    strcpy(comandoLine,hist[3]);
                }
                if( (strcmp(comandoLine, "r 4") == 0)){           
                    printf("%s\n", hist[4] );
                    strcpy(comandoLine,hist[4]);
                }
                if( (strcmp(comandoLine, "r 5") == 0)){           
                    printf("%s\n", hist[5] );
                    strcpy(comandoLine,hist[5]);
                }
                if( (strcmp(comandoLine, "r 6") == 0)){           
                    printf("%s\n", hist[6] );
                    strcpy(comandoLine,hist[6]);
                }
                if( (strcmp(comandoLine, "r 7") == 0)){           
                    printf("%s\n", hist[7] );
                    strcpy(comandoLine,hist[7]);
                }
                if( (strcmp(comandoLine, "r 8") == 0)){           
                    printf("%s\n", hist[8] );
                    strcpy(comandoLine,hist[8]);
                }
                if( (strcmp(comandoLine, "r 9") == 0)){           
                    printf("%s\n", hist[9] );
                    strcpy(comandoLine,hist[9]);
                }
               
	           
			  
			  if(flag3==0){                             // Este flag esta para evitar que el cd que es ejecutado por el chdir
			  analizarOrden(comandoLine, &comando);     // no lo lea nuevamente como otro comando y me arroje comando no encontrado...
			  comando.nombre  = buscarPath(comando.argv, pathv);  // Si comando no es CD entro y analizo el comando, busco su ruta
														// Si no lo encuentra imprimo el msj de error
				if(comando.nombre == NULL)                // comando.nombre
				{
					fprintf(stderr, "Comando %s no encontrado\n", comando.argv[0]);
					continue;                           // No interrumpo el proceso
				}
				/* *********************************************************************
				 * **              PARTE IMPORTANTE DEL PROYECTO                      **
				 * *********************************************************************
				 * Creo un proceso HIJO para que este ejecute el comando el cual va a ejecutar
				 * el hijo, lo analiza, busca la ruta y lo ejecuta, mientras el padre espera a
				 * que su hijo termine...
				 */
				if((chPID = fork()) == 0) execv(comando.nombre, comando.argv);

       
        //---endTime

				thisChPID = wait(&estado);
				
				}
				flag3=0;
				if( (strcmp(comandoLine, "exit") == 0) || (strcmp(comandoLine, "quit") == 0)) break;

         //----Time
       // if(flagTime==1){
        if(strncmp(comandoLine,"time",4)==0 ){
          uswtime(&utime1, &stime1, &wtime1);
          flagTime=0;
          /* --- Cálculo del tiempo de ejecución e impresión de resultados. --- */
          printf("--- \n");
          printf("real  %.4f\n",  wtime1 - wtime0);
          printf("user  %.4f\n",  utime1 - utime0);
          printf("sys   %.4f\n",  stime1 - stime0);
          printf("--- \n");
          printf("CPU/Wall   %.3f %% \n", 100.0 * (utime1 - utime0 + stime1 - stime0) / (wtime1 - wtime0));
          printf("---\n");
          
       }

				

	}

}

void mostrarIndicador() 
{
    /* Construir el indicador con el nombre de la máquina,
    * el directorio actual y cualquier otra información deseada.
    */
    char indicador[] = "JeanC-Miguel@JeanC-Miguel-Desktop$>";
    printf("%s ", indicador);
}

void leerOrden(char *buffer) 
{
    /* Este código utiliza cualquier función E/S, como aquellas de
     * la biblioteca stdio para leer la línea de comando y
     * almacenarla en el buffer. Esta implementación está muy
     * simplificada, pero hace su trabajo.
     */
    fgets(buffer, TAMANO_ORDEN, stdin);   // Ya antes mencionado limpio el caracter optenido por fgets
    buffer[strlen(buffer)-1] = '\0';  
}


int analizarOrden(char *lOrdenes, struct comando_t *orden) 
{
    /* Determina el nombre de la orden y construye la lista de
     * parámetros.* Esta función construirá argv[] y fijara el valor de
     * argc.
     * argc es el número de "tokens" o palabras en la línea de
     * ordenes.
     * argv[] es un array de cadenas de caracteres (apuntadores a
     * char *). El último elemento en argv[] debe ser NULL, según
     * registremos la línea de ordenes desde la derecha, el primer
     * token va en argv[0], el segundo en * argv[l], y así.
     * sucesivamente Cuando añadimos un token a argv[],
     * incrementamos argc.
     */
    int argc;
    int i, j;
    /* Inicialización */
    char **lcPtr;
    lcPtr = &lOrdenes;  /* lOrdenes es la línea de ordenes * Error de Enunciado, Hace falta el &
                         * --
                         * Necesito un Struct comando, ya que este me permitira guarda el nombre del comando
                         * para luego determinar si el nombre de comando sera valida, e ir construyendo 
                         * la lista de parámetros, este me permitira separar los comando ya sea por " .,\t"
                         * segun declare en la Constante ESPACIOBLANCO
                         */
    argc = 0;             

    while((orden->argv[argc++] = strsep(lcPtr, ESPACIOBLANCO)) != NULL) ;
    orden->argv[argc--] = '\0';	
    orden->argc = argc;
    return  1;	
}

int analizarPath(char *dirs[]) 
{
    /* Esta función lee la variable PATH de este entorno,
     * entonces construye un array, dirs[], con los
     * directories en PATH
     */
    int i;
    char *pathEnvVar;
    register char *elPath, *viejaRuta;

    for(i=0; i<MAX_ARGS; i++)
	dirs[i] = NULL;

    pathEnvVar = (char *) getenv("PATH");
    elPath = (char *) malloc(strlen(pathEnvVar) + 1);
    strcpy(elPath, pathEnvVar);
    /* Ciclo para analizar elPath. Buscar un ' : '
     * delimitando cada nombre de path.
     */
    i = 0;
    viejaRuta = elPath;

    for(;; elPath++) 
    {
	if((*elPath == ':') || (*elPath == '\0')) 
        {
            dirs[i] = viejaRuta;
            i++;

            if(*elPath == '\0') break;

            *elPath = '\0';
            viejaRuta = elPath + 1;
        }
    }
}


char *buscarPath(char **argv, char **dir) 
{
    int i;

    /* Esta función busca si el argumento argv[0] (el nombre de
     * archivo) está en los directorios en el argumento dir .
     * Crea una nueva cadena de caracteres, coloca el nombre
     * completo en ellos y devuelva la cadena de caracteres.
     */
    char *resultado;
    char pName[LONG_MAX_PATH];                       // char pName [ LONG_MAX_PATH ];   << ----------------

    // Comprueba si el nombre del archivo ya es un camino absoluto

    if(*argv[0] == '/') 
    {
        resultado = (char *) malloc(strlen(argv[0])+1);
        strcpy(resultado, argv[0]);
        return resultado;
    }

    /* Buscar en los directorios del PATH.
     * Utilizar access() para ver si un archivo está en un dir.
     */

    for(i = 0; i < MAX_PATHS; i++) 
    {
        // Nombre de archivo no encontrado en ningún sitio
        if(dir[i] == NULL) break;
        strcpy(pName, dir[i]);
        strcat(pName, "/");
        strcat(pName, argv[0]);

        if(access(pName, X_OK | F_OK) != -1) 
        {
                            // Si encuentra la ruta con exito, retorno esta ruta...
            resultado = (char *) malloc(strlen(pName)+1);
            strcpy(resultado, pName);
            return resultado;  
        }
    }
    // Nombre de archivo no encontrado en ningún sitio
    fprintf(stderr, "%s: comando no encontrado\n", argv[0]);
    
    return NULL;
}
/* ****************************
 *  Historial de Comandos    **
 * ****************************
 * Este me permite, capturar la señal Crtl^C y me mostrara 
 * solamente el historial de comando...Su funcionamiento lo
 * detallare mas explicitamente en el main, ya que catch solo 
 * Captura Crtl^C y muestra el Historial...
 */
 
void catch(int sig)   
{
	int i = current;
    int hist_num = 0;
	printf("\n");
    do {
        if (hist[i]) {
            printf("r %d %s\n", hist_num, hist[i]);
                hist_num++;
        }
        i = (i + 1) % HISTORY_COUNT;

    } while (i != current);
	 //printf("X_x -+-+--+: Current: %d \n", current);
	//fflush(stdout);
}

int clear_history()
{
        int i;

        for (i = 0; i < HISTORY_COUNT; i++) {
                free(hist[i]);
                hist[i] = NULL;
        }

        return 0;
}



void uswtime(double *usertime, double *systime, double *walltime)
            {
            double mega = 1.0e-6;
            struct rusage buffer;
            struct timeval tp;
            struct timezone tzp;

            getrusage(RUSAGE_SELF, &buffer);
            gettimeofday(&tp, &tzp);
            *usertime = (double) buffer.ru_utime.tv_sec +
                          1.0e-6 * buffer.ru_utime.tv_usec;
            *systime  = (double) buffer.ru_stime.tv_sec +
                          1.0e-6 * buffer.ru_stime.tv_usec;
            *walltime = (double) tp.tv_sec + 1.0e-6 * tp.tv_usec;
            } 