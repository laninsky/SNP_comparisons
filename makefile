SnipIt		: 	SnipIt.o
	g++ -Wall -std=c++11 SnipIt.o -pthread -o SnipIt

SnipIt.o		:	SnipIt.cpp
	g++ -c -Wall -std=c++11 SnipIt.cpp -pthread

clean:
	del *.o
