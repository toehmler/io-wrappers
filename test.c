/*
 * test.c
 *
 * usage: ./test source-file destination-file
 */

#include "myio.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define buf_size 200

int
main(int argc, char *argv[])
{
    /* test program to demonstrate functionality of myio.c
    * copies contents of one file to another */

    struct myfile *in, *out;
    char *in_filename, *out_filename;
    size_t bytes_read, bytes_written;
    char buf[buf_size];

    /* check command-line arguments */
    if (argc != 3) 
	 {
        printf("usage: %s source dest\n", argv[0]);
        exit(1);
    }

    in_filename = argv[1];
    out_filename = argv[2];

    /* open files 
	 * tests functionality of myopen() */
    in = myopen(in_filename, O_RDONLY);
    if (in == NULL) 
	 {
        perror("myopen");
        exit(1);
    }

    out = myopen(out_filename, O_WRONLY | O_CREAT);
    if (out == NULL) 
	 {
        perror("myopen");
        exit(2);
    }

    /* do the copy 
	 * tests functionality of mywrite() and myread() */
    while ((bytes_read = myread(in, buf, buf_size)) > 0) 
	 {
        bytes_written = mywrite(out, buf, bytes_read);
        if (bytes_written < 0)
        {
            perror("mywrite");
            exit(3);
        }
    }

    /* copy 200 bytes starting at new offset
    * tests functionality of myseek() */
   
    off_t new_offset = 10;
    myseek(in, new_offset, SEEK_SET);
    
    bytes_read = myread(in, buf, buf_size);
    if (bytes_read < 0)
    {
        perror("myread");
        exit(3);
    }

    bytes_written = mywrite(out, buf, bytes_read);
    if (bytes_written < 0)
    {
        perror("mywrite");
        exit(3);
    }
	 
    /* clean up 
	 * tests functionality of myclose() and myflush() */
    myclose(in);
    myclose(out);
}

