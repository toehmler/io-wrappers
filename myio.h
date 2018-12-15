/*
  * myio.h
  * Trey Oehmler
  * CS315 Assignment 2 Fall 2018
  * 
  */

#include <string.h>
#include <sys/types.h>

#ifndef __MYIO_H
#define __MYIO_H

#define BUFFER_SIZE 1000 

struct tfile {
    // rdwr = 1: rd last, 2: wr last, 0: neither
    int fd, rw_flag;
    ssize_t file_off, rd_off, rd_count, wr_count;
    char rbuf[BUFFER_SIZE], wbuf[BUFFER_SIZE];
};

struct tfile *myopen(const char *path, int flags);
ssize_t myread(struct tfile *file, void *outbuf, size_t count);
ssize_t mywrite(struct tfile *file, void *inbuf, size_t count);
ssize_t myflush(struct tfile *file);
off_t myseek(struct tfile *file, off_t offset, int whence);
int myclose(struct tfile *file);

#endif

 
