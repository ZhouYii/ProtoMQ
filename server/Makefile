cc=g++
BIN_PATH=bin/
SRC_PATH=src/
OBJECTS= social.pb.o  db_utils.o db_lib.o request_handlers.o worker.o 

clean :
	rm bin/*

all : broker.cpp worker.cpp
	g++ broker.cpp -o $(BIN_PATH)broker -lzmq -lprotobuf
	g++ worker.cpp -o $(BIN_PATH)worker -lzmq -lprotobuf

broker :
	g++ $(SRC_PATH)/broker.cpp -o $(BIN_PATH)broker

db_lib.o : $(SRC_PATH)/db/db_lib.cpp $(SRC_PATH)/db/db_utils.h
	g++ -c $(SRC_PATH)db/db_lib.cpp

db_utils.o : $(SRC_PATH)/db/db_utils.cpp
	g++ -c $(SRC_PATH)db/db_utils.cpp 

request_handlers.o : $(SRC_PATH)/request_handlers.cpp db_lib.o
	g++ -c $(SRC_PATH)/request_handlers.cpp 

worker.o : $(SRC_PATH)/worker.cpp
	g++ -c $(SRC_PATH)/worker.cpp

worker : $(OBJECTS)
	g++ -o $(BIN_PATH)/worker $(OBJECTS) -lzmq -lprotobuf -lcassandra

social.pb.o : $(SRC_PATH)/includes/social.pb.cc
	g++ -c $(SRC_PATH)/includes/social.pb.cc

clean :
	rm bin/* $(OBJECTS)
