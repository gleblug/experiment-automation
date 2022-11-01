CC=g++
CFLAGS=-c
EXECUTABLE=start

all: $(EXECUTABLE)

$(EXECUTABLE): main.o Device.o Generator.o Oscilloscope.o
	$(CC) main.o Device.o Generator.o Oscilloscope.o -o main

main.o:	main.cpp 
	$(CC) $(CFLAGS) main.cpp

Device.o: Device.cpp
	$(CC) $(CFLAGS) Device.cpp

Generator.o: Generator.cpp
	$(CC) $(CFLAGS) Generator.cpp

Oscilloscope.o: Oscilloscope.cpp
	$(CC) $(CFLAGS) Oscilloscope.cpp

clean:
	rm -f main
	rm -f *.o

.PNONY: all clean
