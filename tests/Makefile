CC=emcc
CFLAGS=-I. -O2 -g -Wall -Wdeclaration-after-statement -Wredundant-decls -gsource-map -s INVOKE_RUN=1 -s USE_SDL=2 -s USE_SDL_MIXER=2 -s LEGACY_GL_EMULATION=0 -s USE_SDL_NET=2 -s ASSERTIONS=2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=0 -s FORCE_FILESYSTEM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=['FS','ccall'] -s SAFE_HEAP=1 -s NO_EXIT_RUNTIME=1 -s STACK_OVERFLOW_CHECK=1 --emit-symbol-map --profiling --source-map-base http://0.0.0.0:8080/ -s PROXY_POSIX_SOCKETS=0 -s USE_PTHREADS=1 -s PROXY_TO_PTHREAD=0 -s TOTAL_MEMORY=32MB -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s PTHREAD_POOL_SIZE=2
DEPS = common.h
MKDIR_P = mkdir -p
OUT_DIR = dist
LIBS =

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

all: websockets_client pthreads

websockets_client: websockets_client.o
		${MKDIR_P} ${OUT_DIR}
		$(CC) -o ${OUT_DIR}/websockets_client.html websockets_client.o $(CFLAGS) $(LIBS) -lwebsocket.js

pthreads: pthreads.o
		${MKDIR_P} ${OUT_DIR}
		$(CC) -o ${OUT_DIR}/pthreads.html pthreads.o $(CFLAGS) $(LIBS) -pthread

clean:
		rm -f *.o *~ core *.wasm *.wasm.map *.symbols *.js *.html ${OUT_DIR}/*

