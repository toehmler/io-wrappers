
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __MYIO_H
#define __MYIO_H

#define BUFFER_SIZE 1024

struct myfile
{
  // rdwr = 1: rd last, 2: wr last, 0: neither
  int fd, rend, rdwr;
  char rbuf[BUFFER_SIZE], wbuf[BUFFER_SIZE];
  ssize_t rcount, wcount, offset, r_off;
  int flags;
};

struct myfile *myopen(const char *path, int flags);
ssize_t myread(struct myfile *file, void *usrbuf, size_t count);
ssize_t mywrite(struct myfile *file, void *usrbuf, size_t count);
int myclose(struct myfile *file);
void myflush(struct myfile *file);
off_t myseek(struct myfile *file, off_t offset, int whence);
#endif
 