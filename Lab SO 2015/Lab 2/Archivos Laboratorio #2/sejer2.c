#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main (int argc, char *argv[]) {
 	int i=0;

	char *c;
  	int fd, sz;

 	
 	//printf("\nNombre Ejecutable=%s", argv[0]);
	// PARAMETROS
 	for (i=1; i< argc; i++) {


		c = (char *) calloc(100, sizeof(char));
		fd = open(argv[i], O_RDONLY);
  		if (fd < 0) { perror("r1"); exit(1); }
		sz = read(fd, c, 10);
  		while (  sz == 1){    
  			write(1, &c, 1);    
    			sz = read(0, &c, 1); 
		}
		
		close(fd);

 	}

	printf("\n");
 	return 0;
 }

