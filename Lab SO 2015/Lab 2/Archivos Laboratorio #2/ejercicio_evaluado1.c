#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *inicio(void);

main(void)
{
  char *c;
  int fd, sz, i;	
  c = inicio();

  fd = open("input.in", O_RDONLY);
  if (fd < 0) { perror("r1"); exit(1); }

  sz = read(fd, c, 10);
  c[sz] = '\0';                             //<--------------
  printf("Hemos abierto input.in, y hemos llamado a read(%d, c, 10).\n", fd);
  printf("read ha leido %d bytes.\n", sz);
  printf("Estos bytes son los siguientes: %s\n", c);

  i = lseek(fd, 0, SEEK_CUR);
  printf("lseek(%d, 0, SEEK_CUR) retorna que el desplazamiento actual del archivo es %d\n\n", fd, i);

  printf("Ahora, buscamos el principio del archivo y llamamos a read(%d, c, 10)\n",fd);
  lseek(fd, 0, SEEK_SET);
  sz = read(fd, c, 10);
  c[sz] = '\0';                             //<--------------
  printf("La lectura retorna los siguientes bytes: %s\n", c);

  printf("Ahora, se ejecuta lseek(%d, -6, SEEK_END). y retorna %d\n",fd, (int) lseek(fd, -6, SEEK_END));
  printf("Si ejecutamos read(%d, c, 10), obtendremos los siguientes bytes: ", fd);

  sz = read(fd, c, 10);
  c[sz] = '\0';                             //<--------------
  printf("Finalmente, ejecutamos lseek(%d, -1, SEEK_SET).  Esto retorna -1.\n", fd);
  printf("perror() nos dice por que:\n");
  fflush(stdout);

  i = lseek(fd, 1, SEEK_SET);
  perror("l1");
}

char *inicio(void)
{
  char *bytes;
  int j;
  bytes = (char *) calloc(100, sizeof(char));
  for(j=0;j<100;j++){bytes[j]=rand()%32+1;}
  return bytes;
}

