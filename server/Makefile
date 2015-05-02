cc=g++
BIN_PATH=/bin/
SRC_PATH=src/
OBJECTS=db_lib.o db_utils.o

clean :
	rm bin/*

all : broker.cpp worker.cpp
	g++ broker.cpp -o $(BIN_PATH)broker -lzmq -lprotobuf
	g++ worker.cpp -o $(BIN_PATH)worker -lzmq -lprotobuf

worker :
	g++ $(SRC_PATH)/worker.cpp -o $(BIN_PATH)worker -lzmq -lprotobuf

broker :
	g++ $(SRC_PATH)/broker.cpp -o $(BIN_PATH)broker -lzmq -lprotobuf

db_lib.o : $(SRC_PATH)/db/db_lib.cpp $(SRC_PATH)db/db_lib.h
	g++ -c $(SRC_PATH)db/db_lib.cpp

db_utils.o : $(SRC_PATH)/db/db_utils.cpp $(SRC_PATH)db/db_utils.h
	g++ -c $(SRC_PATH)db/db_utils.cpp

req_handler.o : $(SRC_PATH)/request_handlers.cpp $(SRC_PATH)/request_handlers.h db_lib.o
	g++ -c $(SRC_PATH)/request_handlers.cpp

clean :
	rm bin/* $(OBJECTS)
