#include "elevator.h"

void sig_handler(int sig) {
    if (sig == SIGTERM) {
        exit(0);
    }
}

int find_next_floor(SharedData *shared, int id) {
    Elevator *e = &shared->elevators[id];
    int dir = e->direction;
    if (dir == 0) {
        // Find closest floor with any request
        int min_dist = MAX_FLOORS + 1;
        int closest = -1;
        for (int f = 0; f < MAX_FLOORS; f++) {
            for (int d = 0; d < 2; d++) {
                if (shared->floor_requests[f][d]) {
                    int dist = abs(f - e->current_floor);
                    if (dist < min_dist) {
                        min_dist = dist;
                        closest = f;
                    }
                }
            }
        }
        return closest;
    } else {
        // Find next floor in current direction with request
        int start = e->current_floor + dir;
        int end = (dir == 1) ? MAX_FLOORS : -1;
        int req_dir = (dir == 1) ? 0 : 1; // up for up direction, down for down
        for (int f = start; (dir == 1) ? f < end : f > end; f += dir) {
            if (shared->floor_requests[f][req_dir]) {
                return f;
            }
        }
        return -1;
    }
}

void elevator_process(int id, SharedData *shared) {
    signal(SIGTERM, sig_handler);
    Elevator *e = &shared->elevators[id];
    while (1) {
        pthread_mutex_lock(&shared->mutex);

        // Serve current floor if arrived
        if (e->current_floor == e->target_floor && e->target_floor != -1) {
            // Clear the request
            shared->floor_requests[e->current_floor][0] = 0;
            shared->floor_requests[e->current_floor][1] = 0;
            printf("Elevator %d served floor %d\n", id, e->current_floor);
            e->target_floor = -1;
        }

        // Find next target
        if (e->target_floor == -1) {
            int next = find_next_floor(shared, id);
            if (next != -1) {
                e->target_floor = next;
                e->direction = (next > e->current_floor) ? 1 : ((next < e->current_floor) ? -1 : 0);
            } else {
                // No requests, reverse direction if was moving
                if (e->direction != 0) {
                    e->direction = -e->direction;
                    next = find_next_floor(shared, id);
                    if (next != -1) {
                        e->target_floor = next;
                    } else {
                        e->direction = 0; // idle
                    }
                }
            }
        }

        pthread_mutex_unlock(&shared->mutex);

        // Move
        if (e->target_floor != -1) {
            e->current_floor += e->direction;
            printf("Elevator %d moving to floor %d\n", id, e->current_floor);
        }
        sleep(1); // simulate move time
    }
}