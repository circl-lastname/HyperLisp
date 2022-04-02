CC = gcc

all:
	$(CC) -s -O2 *.c -o hyperlisp

emscripten:
	emcc -s WASM=0 -s ENVIRONMENT=web -O2 *.c -o hyperlisp.js

clean:
	rm -f hyperlisp hyperlisp.js hyperlisp.js.mem

install:
	cp hyperlisp /usr/local/bin
