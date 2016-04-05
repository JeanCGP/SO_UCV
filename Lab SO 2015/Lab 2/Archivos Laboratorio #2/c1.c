#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

main(void)
{
  int fd1, fd2;

  fd1 = open("input.in", O_RDONLY);
  if (fd1 < 0) {
    perror("c1");
    exit(1);
  }

  fd2 = open("input.in", O_RDONLY);
  if (fd2 < 0) {
    perror("c1");
    exit(1);
  }

  printf("archivo input.in abierto dos veces:  descriptores de archivo son %d and %d\n", fd1, fd2);

  if (close(fd1) < 0) {
    perror("c1");
    exit(1);
  }
  if (close(fd2) < 0) {
    perror("c1");
    exit(1);
  }

  printf("cerrados ambos fd's\n");

  fd2 = open("input.in", O_RDONLY);
  if (fd2 < 0) {
    perror("c1");
    exit(1);
  }
  
  printf("reabierto input.in en fd2: %d\n", fd2);

  if (close(fd2) < 0) {
    perror("c1");
    exit(1);
  }

  printf("cerrado fd2.  Ahora, se invoca close(fd2) de nuevo\n");
  printf("Esto causara un error.\n\n");

  if (close(fd2) < 0) {
    perror("c1");
    exit(1);
  }

}

