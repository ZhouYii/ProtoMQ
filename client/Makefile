cc=g++
BIN_PATH="../bin/"

clean :
	rm bin/*

all : client.cpp
	g++ client.cpp includes/social.pb.cc -o client -lzmq -lprotobuf -g
