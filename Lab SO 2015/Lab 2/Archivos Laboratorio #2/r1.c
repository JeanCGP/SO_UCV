#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

main(void)
{
  char *c;
  int fd, sz;

  c = (char *) calloc(100, sizeof(char));

  fd = open("input.in", O_RDONLY);
  if (fd < 0) { perror("r1"); exit(1); }

  sz = read(fd, c, 10);
  printf("llamando read(%d, c, 10).  valor de retorno: %d bytes leidos\n",fd, sz);
  c[sz] = '\0';
  printf("se imprimen los bytes leidos: %s\n", c);

  sz = read(fd, c, 99);
  printf("invocado read(%d, c, 99).  valor de retorno: %d bytes leidos.\n",fd, sz);
  c[sz] = '\0';
  printf("se imprimen los bytes leidos: %s\n", c);

  close(fd);
}
