#include "elevator.h"
#include <sys/wait.h>
#include <signal.h>

int main() {
    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    ftruncate(shm_fd, sizeof(SharedData));
    SharedData *shared = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Initialize shared data
    memset(shared, 0, sizeof(SharedData));
    for (int i = 0; i < MAX_ELEVATORS; i++) {
        shared->elevators[i].id = i;
        shared->elevators[i].current_floor = 0; // ground floor
        shared->elevators[i].direction = 0;
        shared->elevators[i].target_floor = -1;
    }
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared->mutex, &attr);

    // Create message queue
    struct mq_attr mq_attr;
    mq_attr.mq_flags = 0;
    mq_attr.mq_maxmsg = MQ_MAXMSG;
    mq_attr.mq_msgsize = MQ_MSGSIZE;
    mq_attr.mq_curmsgs = 0;
    mqd_t mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0666, &mq_attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    // Fork elevator processes
    pid_t elevator_pids[MAX_ELEVATORS];
    for (int i = 0; i < MAX_ELEVATORS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child: elevator process
            elevator_process(i, shared);
            exit(0);
        } else if (pid > 0) {
            elevator_pids[i] = pid;
        } else {
            perror("fork");
            exit(1);
        }
    }

    // Fork request handler process
    pid_t request_pid = fork();
    if (request_pid == 0) {
        // Child: request handler
        request_handler(shared, mq);
        exit(0);
    } else if (request_pid < 0) {
        perror("fork");
        exit(1);
    }

    // Send some test requests
    Request reqs[] = {
        {5, 0}, // floor 5 up
        {2, 1}, // floor 2 down
        {8, 0}, // floor 8 up
        {1, 0}  // floor 1 up
    };
    for (int i = 0; i < 4; i++) {
        mq_send(mq, (char *)&reqs[i], sizeof(Request), 0);
        sleep(2);
    }

    // Let simulation run
    sleep(30);

    // Terminate processes
    for (int i = 0; i < MAX_ELEVATORS; i++) {
        kill(elevator_pids[i], SIGTERM);
    }
    kill(request_pid, SIGTERM);

    // Wait for processes
    for (int i = 0; i < MAX_ELEVATORS; i++) {
        waitpid(elevator_pids[i], NULL, 0);
    }
    waitpid(request_pid, NULL, 0);

    // Cleanup
    mq_close(mq);
    mq_unlink(MQ_NAME);
    munmap(shared, sizeof(SharedData));
    shm_unlink(SHM_NAME);

    return 0;
}