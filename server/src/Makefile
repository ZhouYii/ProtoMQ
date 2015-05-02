cc=g++
BIN_PATH="../bin/"

clean :
	rm bin/*

all : broker.cpp worker.cpp
	g++ broker.cpp -o $(BIN_PATH)broker -lzmq -lprotobuf
	g++ worker.cpp -o $(BIN_PATH)worker -lzmq -lprotobuf


