CC = gcc
CFLAGS = -Wall -pthread -lrt

all: elevator_system

elevator_system: main.o elevator.o request.o
	$(CC) $(CFLAGS) -o elevator_system main.o elevator.o request.o

main.o: main.c elevator.h
	$(CC) $(CFLAGS) -c main.c

elevator.o: elevator.c elevator.h
	$(CC) $(CFLAGS) -c elevator.c

request.o: request.c elevator.h
	$(CC) $(CFLAGS) -c request.c

clean:
	rm -f *.o elevator_system