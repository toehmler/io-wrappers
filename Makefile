test: test.c
	gcc -Wall -g -pedantic -o test test.c myio.c
	
.PHONY: clean out-file

out-file:
	rm -rf out-file5
	touch out-file5
	chmod 777 out-file5
	
clean:
	rm -rf test
