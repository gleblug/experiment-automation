CC=g++
CFLAGS=-c
EXECUTABLE=start

all: $(EXECUTABLE)

$(EXECUTABLE): main.o Generator.o Oscilloscope.o
	$(CC) main.o Generator.o Oscilloscope.o -o main

main.o:	main.cpp 
	$(CC) $(CFLAGS) main.cpp

Generator.o: Generator.cpp Generator.hpp
	$(CC) $(CFLAGS) Generator.cpp

Oscilloscope.o: Oscilloscope.cpp Oscilloscope.hpp
	$(CC) $(CFLAGS) Oscilloscope.cpp

clean:
	rm -f main
	rm -f *.o

.PNONY: all clean
