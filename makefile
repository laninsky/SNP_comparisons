SnipIt		: 	SnipIt.o
	g++ -Wall SnipIt.o -pthread -o SnipIt

SnipIt.o		:	SnipIt.cpp sem.h
	g++ -c -Wall SnipIt.cpp -pthread

clean:
	del *.o
