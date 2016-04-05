/* ****************************************************************
 * ** Jean Carlos Gomes   ** C.I.V-19.162.619 ** SEC C2 (Pedro:()**
 * ****************************************************************
 * ** Laboratorio 2 - Uso de Fork - vFork - NamePipe -Ejercico 2 **
 * ****************************************************************
 * ** ejercicio2.c : Realice un código en lenguaje C en el cual un 
 * ** proceso hijo escriba su identificador de proceso en una tubería 
 * ** con nombre, el proceso padre deberá leerlo e imprimirlo en salida
 * ** estándar.
 * ****************************************************************
 */
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


int main()
{
	
	pid_t idProceso;
	int estadoHijo;
	int descriptorTuberia[2];
	char buffer[100];

	if (pipe (descriptorTuberia) == -1)
	{
		perror ("No se puede crear Tuberia");
		exit (-1);
	}
	
	
	idProceso = fork();

	if (idProceso == -1)  // falla en el fork no se creo el hijo
	{
		perror ("No se puede crear proceso");
		exit (-1);
	}


	if (idProceso > 0) // PADRE
	{
		
		close (descriptorTuberia[1]);

		
		read (descriptorTuberia[0], buffer, 5);
		printf ("PADRE  : Recibido \"%s\"\n", buffer);
		printf ("ID PADRE es : %d\n", getpid());
		exit (0);
	}

	
	if (idProceso == 0) // Hijo
	{
		
		close (descriptorTuberia[0]);


		printf ("HIJO : Envio %d \n",getpid());
		//strcpy (buffer, (char)getppid()); !!NO FUNKA!!
		sprintf(buffer, "%d", getpid());
		write (descriptorTuberia[1], buffer, strlen(buffer)+1);

		
		wait (&estadoHijo);
		return 0;
	}
return 0;
}

