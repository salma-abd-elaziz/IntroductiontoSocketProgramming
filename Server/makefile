all:
	g++ -std=c++11 -c functions.cpp -o functions.o 
	g++ -std=c++11 -o server server.cpp functions.o

clear:
	rm server
	rm functions.o