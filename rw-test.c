/* 
 * rw-test.c
 * Trey Oehmler
 * CS315 Assignment 2 Fall 2018
 *
 * tests rdwr (single file) functionality of myio.c
 *
 */

#include "myio.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define BUF_SIZE 15 

int 
main(int argc, char *argv[])
{
    struct tfile *file;
    char *file_name;
    char buf[BUF_SIZE];

    if (argc != 2) {
        exit(1);
    }

    file_name = argv[1];
    file = myopen(file_name, O_RDWR);

    myread(file, buf, 3); 
    mywrite(file, buf, 3); // replace 2s
    myread(file, buf, 3);
    mywrite(file, buf, 3); // replace 4s
    myread(file, buf, 3);
    mywrite(file, buf, 3); // replace 5s

    myseek(file, 2, SEEK_SET);
    myread(file, buf, 3);
    myseek(file, 15, SEEK_CUR);
    mywrite(file, buf, 3);
    
    myclose(file);

    return 0;
}
