cc=gcc
BIN_PATH="../bin/"

clean :
	rm bin/*

all : broker.c worker.c
	$(CC) -o $(BIN_PATH)broker broker.c -lzmq
	$(CC) -o $(BIN_PATH)worker worker.c -lzmq
