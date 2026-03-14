#include "elevator.h"

void request_handler(SharedData *shared, mqd_t mq) {
    signal(SIGTERM, sig_handler);
    Request req;
    while (1) {
        ssize_t bytes = mq_receive(mq, (char *)&req, MQ_MSGSIZE, NULL);
        if (bytes == -1) {
            perror("mq_receive");
            continue;
        }
        pthread_mutex_lock(&shared->mutex);
        shared->floor_requests[req.floor][req.direction] = 1;
        printf("Request: floor %d, direction %s\n", req.floor, req.direction ? "down" : "up");
        pthread_mutex_unlock(&shared->mutex);
    }
}