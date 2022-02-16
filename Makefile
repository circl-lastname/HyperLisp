all:
	gcc -s -O2 lexer.c parser.c main.c -o hyperlisp

clean:
	rm hyperlisp

install:
	cp hyperlisp /usr/local/bin
