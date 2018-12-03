test: test.c
	gcc -Wall -g -pedantic -o test test.c newio.c

test-new: test-new.c
	gcc -Wall -g -pedantic -o test-new test-new.c newio.c

	
.PHONY: clean out-file

out-file:
	rm -rf sample-out
	touch sample-out 
	sudo chmod 777 sample-out

clean:
	rm -rf test
