all:
	gcc -s -O2 shared.c lexer.c parser.c debug.c main.c -o hyperlisp

emscripten:
	emcc -s WASM=0 -s ENVIRONMENT=web -O2 shared.c lexer.c parser.c debug.c main.c -o hyperlisp.js

clean:
	rm -f hyperlisp
	rm -f hyperlisp.js
	rm -f hyperlisp.js.mem

install:
	cp hyperlisp /usr/local/bin
