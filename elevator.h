#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <pthread.h>
#include <mqueue.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h> 

#define MAX_ELEVATORS 3
#define MAX_FLOORS 10
#define MQ_NAME "/elevator_mq"
#define SHM_NAME "/elevator_shm"
#define MQ_MAXMSG 10
#define MQ_MSGSIZE sizeof(Request)

typedef struct {
    int floor;
    int direction; // 0 for up, 1 for down
} Request;

typedef struct {
    int id;
    int current_floor;
    int direction; // 0 idle, 1 up, -1 down
    int target_floor;
} Elevator;

typedef struct {
    Elevator elevators[MAX_ELEVATORS];
    int floor_requests[MAX_FLOORS][2]; // [floor][0]=up, [1]=down
    pthread_mutex_t mutex;
} SharedData;

// Function declarations
void sig_handler(int sig);
void elevator_process(int id, SharedData *shared);
void request_handler(SharedData *shared, mqd_t mq);

#endif