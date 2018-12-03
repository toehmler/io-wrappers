 /*
  * myio.c
  * Trey Oehmler
  * CS315 Assignment 2 Fall 2018
  * 
  */

#include "myio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct myfile *myopen(const char *path, int flags)
{
    /* 
- Still needs to support flags 
*/
    struct myfile *file = malloc(sizeof(*file));
    if (file == NULL)
    {
        // print error here?
        return NULL;
    }
    
    if (flags == O_RDWR)
    {
        file->flags = O_RDWR;
        file->rcount = 0;
        file->wcount = 0;
    }
    else if (flags == O_RDONLY) 
    {
        file->flags = O_RDONLY;
        file->rcount = 0;
    }
    else if (flags == O_WRONLY) 
    {
        file->flags = O_WRONLY;
        file->wcount = 0;
    }

    file->fd = open(path, flags);
    file->offset = 0;
    return file;
    if (file->fd == -1)
    {
        perror("open");
        return NULL;
    }
}

ssize_t myread(struct myfile *file, void *usrbuf, size_t count)
{
    
    int bytes_read;
    if (file->rcount < count) 
    // fill up file->rbuf if not enough bytes
    {   
        bytes_read = read(file->fd, file->rbuf, buffer_size - file->rcount);
        if (bytes_read == -1)
        {
          
            perror("read");
            return -1;
        }
        file->rcount += bytes_read;
        
      
    }
    // copy bytes to usr supplied buffer
    memcpy(usrbuf, file->rbuf, count);
    file->offset += count;
    if (file->rcount >= count)
    {
        file->rcount -= count;
        return count;
    }
    file->rcount = 0;
    return file->rcount;
}

ssize_t mywrite(struct myfile *file, void *usrbuf, size_t count)
{
    if (file->wcount < buffer_size)
    {
        memcpy(file->wbuf, usrbuf, count);
        file->wcount += count;
        file->offset += count;
    }
    else 
    {
        printf("%d\n", file->fd);
        int bytes_written = write(file->fd, file->wbuf, buffer_size);
        if (bytes_written == -1) 
        {
            perror("write");
            return bytes_written;
        }
        file->wcount = 0;
    }
    return file->wcount; 
    
}

int myclose(struct myfile *file)
{
    close(file->fd);
    return 0;
    //returns zero on success otherwise returns -1 and sets errno
}

off_t myseek(struct myfile *file, off_t offset, int whence)
{
    if (whence == SEEK_SET)
    {
        return lseek(file->fd, offset, whence);
    }
    else
    {
        if (whence == SEEK_CUR) 
        {
            if (file->flags == O_WRONLY || file->flags == O_RDWR)
            {
                if (file->wcount + offset > buffer_size)
                {
                    file->wcount = 0;
                    
                    off_t diff = offset - (buffer_size - file->wcount);
                    return lseek(file->fd, diff, whence);
                }
                else 
                {
                    file->wcount += offset;
                    file->offset += offset;
                    return file->offset;                    
                    // not sure how to return locataion relative to beginning of file
                }
            }
            else if (file->flags == O_RDONLY || file->flags == O_RDWR) 
            {
                if (file->rcount + offset > buffer_size)
                {
                    file->rcount = 0;
                    file->offset += offset;
                    off_t diff = offset - (buffer_size - file->rcount);
                    return lseek(file->fd, diff, whence);
                }
                else 
                {
                    file->rcount += offset;
                    file->offset += offset;
                    return file->offset;
                }
            }
            else 
            {
                return -1;
            }
        }
        else 
        {
            return -1;
        }
    }
}

void myflush(struct myfile *file, void *usrbuf)
{
    write(file->fd, usrbuf, buffer_size - file->wcount);
    // forces any data within wbuf of file struct to actually be written
    // returns the number of bytes written
}
