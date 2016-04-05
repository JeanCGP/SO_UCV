#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

main(void)
{
  int fd;

  fd = open("input.in", O_RDONLY);
  printf("%d\n", fd);
}

