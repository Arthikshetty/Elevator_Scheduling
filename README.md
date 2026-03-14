# Elevator Scheduling System

This project simulates an elevator scheduling system using the SCAN algorithm in POSIX-compliant C. It uses POSIX message queues for inter-process communication, shared memory for data sharing, and mutex-based synchronization for thread safety.

## Features

- Multiple elevators (default: 3)
- SCAN algorithm for elevator scheduling
- POSIX message queues for request handling
- Shared memory for elevator states and floor requests
- Mutex synchronization for concurrent access

## Building

Ensure you have a POSIX-compliant environment (Linux, macOS, or WSL on Windows) with GCC and POSIX libraries.

```bash
make
```

## Running

```bash
./elevator_system
```

The program will simulate elevators responding to requests. It sends some test requests and runs for 30 seconds before terminating.

## Files

- `elevator.h`: Header file with definitions and structures
- `main.c`: Main program that sets up processes and shared resources
- `elevator.c`: Elevator process logic implementing SCAN algorithm
- `request.c`: Request handler process that receives and sets floor requests
- `Makefile`: Build script

## Algorithm

The SCAN algorithm (elevator algorithm) works as follows:
- Elevators move in one direction serving requests until no more requests in that direction.
- When no more requests in the current direction, the elevator reverses direction.
- If idle, the elevator goes to the closest floor with a pending request.

## Inter-Process Communication

- **Message Queues**: Used to send floor requests from the main process to the request handler.
- **Shared Memory**: Holds the state of elevators and floor requests, accessible by all processes.
- **Mutexes**: Protect shared memory from concurrent access.