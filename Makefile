CFLAGS=-Wall -g -pedantic

copy-test: copy-test.c myio.c
	gcc $(CFLAG) -o copy-test copy-test.c myio.c

rw-test: rw-test.c myio.c
	gcc $(CFLAGS) -o rw-test rw-test.c myio.c

.PHONY: clean
clean:
	rm -rf copy-test
	rm -rf rw-test
