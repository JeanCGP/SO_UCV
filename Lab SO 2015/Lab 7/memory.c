#include "stdio.h"

int max_fisicas,max_virtuales,marcos_usados,fallos=0;
int N;

struct info// estructura de control de procesos
{
	int marco;
	int tvz;	
};
struct info marcos[1024];

int pos=0;

int viejo=0;

void Vejez()
{
	int i;
	for(i=0; i<max_virtuales; i++)
	{ 
		marcos[i].tvz=marcos[i].tvz+1;
	}
}

//Algoritmo de reemplazo LRU---------------------------------------------------
void Reemplazo(int peticion)
{
	int k;
	for(k=0; k<max_virtuales; k++) 
	{ if (marcos[k].tvz>=marcos[pos].tvz) {pos=k;} }
	
	Vejez();
	int j;
	for(j=0; j<max_virtuales; j++)
	{ 	if(j==pos){marcos[j].tvz=0;}
	}	
	
	marcos[pos].marco=peticion;
	
	pos++;
	fallos++;
	int i;
	printf("\nArreglo de marcos virtuales:\n[");
	for(i=0;i<max_virtuales;i++)
	{
		printf("%i,",marcos[i].marco);
	}
	printf("]\n");
}
//Algoritmo de reemplazo LRU---------------------------------------------------

int main(int argc, char* argv[] )
{
	//int x;
	if(argc<4){ printf("Cantidad de parametros invalidos\n"); printf("La sintaxis es:%s [#proc. fisicos maximo] [#maximo de marcos] [nombre archivo peticiones]\n",argv[0]);return 0;}
	max_fisicas=atoi(argv[1]);
	max_virtuales=atoi(argv[2]);

	FILE* f;
	f=fopen(argv[3],"r");
	if(f==NULL)
	{
		printf("Error, no se puede abrir el archivo %s\n",argv[3]);
		return 0;
	}
	fscanf(f,"%d",&N);//N son la cantidad maxima de peticiones que se hacen por el archivo
	int i,j,peticion,encontrado;

	marcos_usados=0;

	//Comienzan las peticiones leidas del archivo
	for(i=0;i<N;i++)
	{
		fscanf(f,"%d",&peticion);
		if(peticion>max_fisicas)
		{
			printf("Error, peticion excede del numero maximo de paginas fisicas\n");
			return 0;
		}
		encontrado=0;
		printf("Se solicita la pagina %i\n",peticion);
		for(j=0;j<max_virtuales;j++)
		{
			if(marcos[j].marco==peticion) 
			{
				encontrado=1; 
				Vejez(); 
				int z;
				for(z=0; z<max_virtuales; z++)
				{ 	if(z==j){marcos[j].tvz=0;}
				}	
				break;}
		}
		if(encontrado==1){printf("Elemento encontrado!!\n");}
		if( (marcos_usados<max_virtuales)&&(encontrado==0) )
		{
		//Como hay espacio en los marcos se agrega al final de estos
		
		//for(x=0; x<max_virtuales; x++) 
		//{ if (marcos[x].tvz>viejo) {pos=x;} }
		
			marcos[marcos_usados].marco=peticion;
			Vejez();
			int z;
				for(z=0; z<max_virtuales; z++)
				{ 	if(z==marcos_usados){marcos[z].tvz=0;}
				}	
			
			marcos_usados++;
			printf("Se agrega al marco \n");
			continue;
		}
		if(encontrado==0) {Reemplazo(peticion);printf("Hay un fallo, se procede a un intercambio\n");}

	}
	printf("\nLa cantidad de fallos de pagina que hubo fueron de %d\n",fallos);
	fclose(f);
	return 0;
}
