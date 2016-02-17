all:
	g++ -std=c++11 -ggdb -o chat ChatMultiplexing/MULTITHREAD.cpp
	g++ -std=c++11 -ggdb -o sigign IPC/src/sigignore.cpp
	g++ -std=c++11 -o pipe PIPE/mini_bash(PIPE).cpp
clean:
	rm -rf *.o chat sigign 
