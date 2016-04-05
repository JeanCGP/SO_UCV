#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

main(void)
{
  int fd, sz;

  fd = open("salida.out", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) { perror("r1"); exit(1); }

  sz = write(fd, "GDSO\n", strlen("GDSO\n"));

  printf("llamando write(%d, \"GDSO\\n\", %d).  valor de retorno %d\n",fd, strlen("GDSO\n"), sz);

  close(fd);
}

