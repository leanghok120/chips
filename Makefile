all:
	gcc -o chips main.c -lncurses

install:
	mv chips /usr/local/bin
