all:http_parser.o server.o
	g++ http_parser.o MULTITHREAD.o -o server -pthread
	rm *.o
http_parser.o: http_parser.c http_parser.h
	g++ -c http_parser.c
server.o: MULTITHREAD.cpp
	g++ -std=c++11 -c MULTITHREAD.cpp
