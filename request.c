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

        int best_elevator = -1;
        int min_distance = MAX_FLOORS + 1;

        // Find closest idle elevator
        for (int i = 0; i < MAX_ELEVATORS; i++) {
            int dist = abs(shared->elevators[i].current_floor - req.floor);

            if (dist < min_distance && shared->elevators[i].target_floor == -1) {
                min_distance = dist;
                best_elevator = i;
            }
        }

        if (best_elevator != -1) {

            shared->elevators[best_elevator].target_floor = req.floor;

            if (req.floor > shared->elevators[best_elevator].current_floor)
                shared->elevators[best_elevator].direction = 1;
            else
                shared->elevators[best_elevator].direction = -1;

            printf("Request floor %d assigned to Elevator %d\n",
                   req.floor, best_elevator);
        }

        pthread_mutex_unlock(&shared->mutex);
    }
}