cc=g++
BIN_PATH=/bin/
SRC_PATH=src/
OBJECTS=db_lib.o db_utils.o request_handlers.o worker.o

clean :
	rm bin/*

all : broker.cpp worker.cpp
	g++ broker.cpp -o $(BIN_PATH)broker -lzmq -lprotobuf
	g++ worker.cpp -o $(BIN_PATH)worker -lzmq -lprotobuf

broker :
	g++ $(SRC_PATH)/broker.cpp -o $(BIN_PATH)broker -lzmq -lprotobuf

db_lib.o : $(SRC_PATH)/db/db_lib.cpp $(SRC_PATH)db/db_lib.h
	g++ -c $(SRC_PATH)db/db_lib.cpp -lzmq -lprotobuf

db_utils.o : $(SRC_PATH)/db/db_utils.cpp $(SRC_PATH)db/db_utils.h
	g++ -c $(SRC_PATH)db/db_utils.cpp -lzmq -lprotobuf

request_handlers.o : $(SRC_PATH)/request_handlers.cpp $(SRC_PATH)/request_handlers.h db_lib.o
	g++ -c $(SRC_PATH)/request_handlers.cpp -lzmq -lprotobuf

worker.o : $(SRC_PATH)/worker.cpp $(SRC_PATH)/worker.h
	g++ -c $(SRC_PATH)/worker.cpp -lzmq -lprotobuf

worker : $(OBJECTS)
	#g++ $(SRC_PATH)/worker.cpp -o $(BIN_PATH)worker -lzmq -lprotobuf
	g++ -o $(BIN_PATH)/worker $(OBJECTS)

clean :
	rm bin/* $(OBJECTS)
