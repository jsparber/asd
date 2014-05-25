distance: main.c makefile
	gcc -ansi -Wall -O main.c -o distance

clean:
	rm -f main.o

clean-all:
	rm -f main.o distance
