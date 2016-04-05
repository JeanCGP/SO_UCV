#include <stdio.h>
#include <ctype.h>

char* Mstrupr(char* szCad); 

int main()
{
	char szCadena[] = "Esto es una cadena";
	char szCadena2[] = "Esto es otra cadena";

	printf("%s\n", Mstrupr(szCadena));
	printf("%s\n", Mstrupr(szCadena2));
	return 0;
}

char* Mstrupr(char* szCad) 
{
	int i;
	for (i=0; szCad[i]; i++) 
		szCad[i] = toupper(szCad[i]);
	return szCad;
}


// la función Mstrupr no recibe literales, por lo tanto se debe almacenar la cadena "Esto es otra cadena" en una variable y pasarle como parametro 
// a la función esa variable.
