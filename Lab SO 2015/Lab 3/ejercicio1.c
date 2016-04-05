/* ****************************************************************
 * ** Jean Carlos Gomes   ** C.I.V-19.162.619 ** SEC C2 (Pedro:()**
 * ****************************************************************
 * ** Laboratorio 2 - Uso de Fork - vFork - NamePipe -Ejercico 1 **
 * ****************************************************************
 * ** ejercicio1.c : Realice un código en lenguaje C en el cual se creen dos procesos hijos, cada uno leerá
 * una cadena de entrada estándar y la imprimirá carácter a carácter, finalmente, el proceso
 * padre se encargará de imprimir la concatenación de ambas cadenas por salida estándar. 8
 * puntos.
 * ****************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

main()
{
   char *sIdentifier;
   char a[50];
   char b[50];
   pid_t pID = vfork();

   if (pID == 0)                // Hijo
   {
      // Code only executed by child process
	 
     	sIdentifier="Child1 Process: ";
	scanf("%s",a);
	int i;
	for(i=0;i<strlen(a);i=i+1){
		printf("%c\n",a[i]);
	}printf("\n");

	exit(0);
    }

	pid_t pID1 = vfork();

   if (pID1 == 0)                // Hijo
   {
      // Code only executed by child process
	 
     	sIdentifier = "Child2 Process: ";
	scanf("%s",b);
	int i;
	for(i=0;i<strlen(b);i=i+1){
		printf("%c\n",b[i]);
	}printf("\n");

	exit(0);
    }


    else if (pID < 0)            // falla en el fork
    {
       // cerr << "Failed to fork" << endl;
        exit(1);
        // Throw exception
    }
 

   else                                   // parent
    {
     sIdentifier = "Parent Process:";
     strcat(a,b);
	int i;
	for(i=0;i<strlen(a);i=i+1){
		printf("%c",a[i]);
	}printf("\n");

    }

    // executed only by parent


    exit(0);
}
