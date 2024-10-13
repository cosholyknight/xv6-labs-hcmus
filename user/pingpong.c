#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
    if (argc != 1) {
        printf("Usage: pingpong\n");
        exit(-1);
    }

    // Create pipe
    int p[2];

    // Check pipe initialization error
    if (pipe(p) < 0) {
        printf("pipe initialization failed");
        exit(-1);
    }
    char buffer;
    int pid = fork();
    // Check fork error
    if (pid < 0) {
        printf("fork failed");
    }

    if (pid == 0) {
        // Child process

        // Read from pipe
        if (read(p[0], &buffer, sizeof(buffer)) < 0) {
            printf("read failed");
            exit(-1);
        }

        printf("%d: received ping\n", getpid());

        // Write pipe
        if (write(p[1], "a", 1) < 0) {
            printf("write failed");
            exit(-1);
        }
        // close pipe
        close(p[0]);
        close(p[1]);
    }
    else {
        // Parent process

        // Write pipe
        if (write(p[1], "b", 1) < 0) {
            printf("write failed");
            exit(-1);
        }
        // Wait for the child process to finish
        wait(0);
        // Read from pipe
        if (read(p[0], &buffer, sizeof(buffer)) < 0) {
            printf("read failed");
            exit(-1);
        }
        printf("%d: received pong\n", getpid());
        //close pipe
        close(p[0]);
        close(p[1]);
    }
    exit(0);
}