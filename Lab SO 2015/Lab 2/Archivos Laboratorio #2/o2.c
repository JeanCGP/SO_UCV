#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

main(void)
{
  int fd;

  fd = open("out1", O_WRONLY);
  if (fd < 0) {
    perror("out1");
    exit(1);
  }
}

