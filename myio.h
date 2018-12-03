/*
  * myio.h
  * Trey Oehmler
  * CS315 Assignment 2 Fall 2018
  * 
  */

#include <unistd.h>
#ifndef __MYIO_H
#define __MYIO_H

#define buffer_size 200

struct myfile
{
  int fd;
  char rbuf[buffer_size], wbuf[buffer_size];
  ssize_t rcount, wcount, offset;
  int flags;
};

struct myfile *myopen(const char *path, int flags);
ssize_t myread(struct myfile *file, void *usrbuf, size_t count);
ssize_t mywrite(struct myfile *file, void *usrbuf, size_t count);
int myclose(struct myfile *file);
void myflush(struct myfile *file, void *usrbuf);
off_t myseek(struct myfile *file, off_t offset, int whence);

#endif
