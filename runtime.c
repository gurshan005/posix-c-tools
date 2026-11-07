#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fputs("usage: runtime COMMAND [ARG ...]\n", stderr);
        return 2;
    }

    struct timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
        perror("clock_gettime");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        execvp(argv[1], argv + 1);
        int code = errno == ENOENT ? 127 : 126;
        perror(argv[1]);
        _exit(code);
    }

    int status;
    pid_t waited;
    do {
        waited = waitpid(pid, &status, 0);
    } while (waited < 0 && errno == EINTR);
    if (waited < 0) {
        perror("waitpid");
        return 1;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == 0) {
        double seconds = (double)(end.tv_sec - start.tv_sec)
            + (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
        fprintf(stderr, "elapsed %.6f s\n", seconds);
    } else {
        perror("clock_gettime");
    }

    if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    return WEXITSTATUS(status);
}
