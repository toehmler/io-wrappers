/*
 * copy-file.c
 *
 * usage: ./copy-file source-file destination-file
 */

#include "newio.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define buf_size 200

int
main(int argc, char *argv[])
{

    struct myfile *in, *out;
    char *in_filename, *out_filename;
    size_t bytes_read, bytes_written;
    char buf[buf_size];

    /* check command-line arguments */
    if(argc != 3) {
        printf("usage: %s source dest\n", argv[0]);
        exit(1);
    }
    in_filename = argv[1];
    out_filename = argv[2];

    /* open files */
    in = myopen(in_filename, O_RDONLY);
    if(in == NULL) {
        perror("myopen");
        exit(1);
    }
    out = myopen(out_filename, O_WRONLY);
    if(out == NULL) {
        perror("myopen");
        exit(2);
    }

    /* do the copy */
    while((bytes_read = myread(in, buf, buf_size)) > 0) {
        printf("bytes read: %zu\n", bytes_read);
        bytes_written = mywrite(out, buf, bytes_read);
        printf("bytes writen: %zu\n", bytes_written);
        if (bytes_written == 0)
        {
            perror("mywrite");
            exit(3);
        }
    }
   




    /* the while loop above could terminate if fread encountered an error, so
     * check that and report if necessary */
    /*
    if(ferror(in)) {
        perror("myread");
        exit(4);  
    }
    */
   

    /* clean up */
    myclose(out);
    myclose(in);
}

