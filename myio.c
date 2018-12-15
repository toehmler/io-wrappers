/* 
 * myio.c
 * Trey Oehmler
 * CS315 Assignment 2 Fall 2018
 */

#include "myio.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <string.h>

struct tfile *myopen(const char *path, int flags)
{
    /* initalizes new tfile
     * returns ptr to tfile, or NULL on err */
    int fd = open(path, flags, 0666);
    if (fd == -1) {
        return NULL;
    }

    struct tfile *ret_file = malloc(sizeof(struct tfile));
    if (!ret_file) {
        return NULL; // malloc(2) error
    }

    ret_file->fd = fd;
    ret_file->rw_flag = 0;
    ret_file->file_off = 0;
    ret_file->rd_off = 0;
    ret_file->rd_count = 0;
    ret_file->wr_count = 0;

    return ret_file;
}

off_t myseek(struct tfile *file, off_t offset, int whence) 
{
    /* repositions file offset by calling lseek(2)
     * absolute or relative dependent on whence 
     * rets new offset or -1 on error */
    off_t new_off;

    if (file->rw_flag == 2) {
        myflush(file);
    } 
    if (whence == SEEK_SET) {
        new_off = lseek(file->fd, offset, SEEK_SET);
    } else { // whence == SEEK_CUR
        new_off = lseek(file->fd, (offset + file->file_off), SEEK_SET);
    }
    if (new_off != -1) {
        file->file_off = new_off;
        file->rd_count = 0;
        file->rd_off = 0;
        file->rw_flag = 0;
    }

    return new_off;
}

ssize_t myflush(struct tfile *file)
{
    /* write any data in wbuf to file 
     * rets number of bytes written or -1 on err */
    if (file->wr_count == 0) {
        return 0;
    }
    ssize_t bytes_wr = write(file->fd, file->wbuf, file->wr_count);
    if (bytes_wr != -1) {
        file->file_off += bytes_wr;
        file->wr_count = 0;
    }
    return bytes_wr;
}

ssize_t myread(struct tfile *file, void *outbuf, size_t count) 
{
    /* calls read() to fill rbuf of file
     * copies data to outbuf
     * returns number of bytes copied to outbuf */
    ssize_t bytes_rd;

    if (file->rw_flag == 2) {
        myflush(file);
    }
    file->rw_flag = 1; 
    
    int increment;    
    if (count <= file->rd_count) {
        // copy to usrbuf from rbuf
        memcpy(outbuf, (char *)file->rbuf + file->rd_off, count);
        increment = count;
    } else if (count <= BUFFER_SIZE) {
        // move rem bytes to start of rbuf
        memmove(file->rbuf, (char *)file->rbuf + file->rd_off, file->rd_count);
        file->rd_off = 0;
        // call read(2) to fill rbuf
        size_t rd_req = BUFFER_SIZE - file->rd_count;
        bytes_rd = read(file->fd, (char *)file->rbuf + file->rd_count, rd_req);
        if (bytes_rd == -1) {
            return -1;
        }
        // copy to usrbuf from rbuf
        file->rd_count += bytes_rd;
        if (count > file->rd_count) {
            memcpy(outbuf, file->rbuf, file->rd_count);
            increment = file->rd_count;
        } else { 
            memcpy(outbuf, file->rbuf, count);
            increment = count;
        }
    } else { // count > BUFFER_SIZE
        // call lseek(2) to reposition offset 
        lseek(file->fd, file->file_off, SEEK_SET);
        // call read(2) to fill usrbuf w count bytes
        bytes_rd = read(file->fd, outbuf, count);
        if (bytes_rd == -1) {
            return -1;
        } 
        // update offs / count for special case
        file->file_off += bytes_rd;
        file->rd_count = 0;
        file->rd_off = 0;
        return bytes_rd;
    }
    // update file_off, rd_off, rd_count
    file->file_off += increment;
    file->rd_count -= increment;
    file->rd_off += increment;
    return (ssize_t)increment;
}

ssize_t mywrite(struct tfile *file, void *inbuf, size_t count)
{
    /* copies from inbuf to wbuf of file
     * write to file when neccesary
     * rets bytes written or -1 on error */
    ssize_t bytes_wr;

    if (file->rw_flag == 1) {
        lseek(file->fd, file->file_off, SEEK_SET);
        file->rd_count = 0;
        file->rd_off = 0;
    } 

    file->rw_flag = 2;

    if ((count + file->wr_count) > BUFFER_SIZE) {
        // write any remaining data in wbuf to file
        if (file->wr_count > 0) {
            bytes_wr = write(file->fd, file->wbuf, file->wr_count);
            if (bytes_wr == -1) {
                return -1; // write(2) error
            } 
            file->file_off += bytes_wr;
            file->wr_count = 0;
        }
    }

    // perform syscall directly for overflow count
    if (count > BUFFER_SIZE) {
        bytes_wr = write(file->fd, inbuf, count);
        if (bytes_wr == -1) {
            return -1; // write(2) error
        }
        file->file_off += bytes_wr;
        return bytes_wr;
    } 

    char *wr_start = (char *)file->wbuf + file->wr_count;
    memcpy(wr_start, inbuf, count);
    file->wr_count += count;
    return count;
}

int myclose(struct tfile *file) 
{
    /* flushes any remaining data in wbuf
     * closes a file by calling close(2) 
     * returns 0 on successful close, -1 otherwise */
    myflush(file);
    int ret = close(file->fd);
    if (ret < 0) {
        return -1;
    } else {
        free(file);
        return 0;
    }
}



